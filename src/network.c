#include "network.h"
#include "core.h"
#include "editor.h"
#include "items.h"

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define __USE_XOPEN2K
#include <netdb.h>

bool network_start(Network *n, const char *host, const char *port, NetworkMode mode)
{
    switch ((n->mode = mode))
    {
    case CLIENT:
        n->file = network_start_client(host, port);
        return n->file > 0;
    case SERVER:
        n->file = network_start_server(host, port);
        return n->file > 0;
    default:
        return true;
    }
}

int network_start_client(const char *host, const char *port)
{
    struct addrinfo hints = {0}, *results;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int code = getaddrinfo(host, port, &hints, &results);
    if (code)
    {
        set_status("getaddrinfo failed: %s", gai_strerror(code));
        return -1;
    }

    int file = -1;
    for (struct addrinfo *p = results; p; p = p->ai_next)
    {
        if ((file = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        if (connect(file, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(file);
            file = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(results);
    return file;
}

int network_start_server(const char *host, const char *port)
{
    struct addrinfo hints = {0}, *results;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int code = getaddrinfo(host, port, &hints, &results);
    if (code)
    {
        set_status("getaddrinfo failed: %s", gai_strerror(code));
        return -1;
    }

    const int yes = 1;
    int file = -1;
    for (struct addrinfo *p = results; p; p = p->ai_next)
    {
        if ((file = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        // This allows us to use the address again.
        if (setsockopt(file, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            goto error;

        if (bind(file, p->ai_addr, p->ai_addrlen) < 0)
            goto error;

        if (listen(file, 20) < 0)
        {
        error:
            close(file);
            file = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(results);
    return file;
}

void network_stop(Network *n)
{
    clients_reset(&n->clients);
    if (n->file > 0)
    {
        close(n->file);
        n->file = -1;
    }
}

void network_add_files_set(Network *n, fd_set *set, int *max)
{
    if (n->file > 0)
    {
        *max = (*max < n->file) ? n->file : *max;
        FD_SET(n->file, set);
    }

    for (int i = 0; i < n->clients.size; i++)
    {
        *max = (*max < n->clients.data[i]) ? n->clients.data[i] : *max;
        FD_SET(n->clients.data[i], set);
    }
}

void network_handle(Network *n, int file, Editor *e)
{
    if ((n->mode == SERVER) && (file == n->file))
    {
        int client = accept(file, NULL, NULL);
        if (client < 0)
        {
            set_status("Failed to accept new connection");
            return;
        }

        clients_append(&n->clients, client);
        set_status("Got new connection on descriptor %d", client);
    }
    else
    {
        short op;

        if (read(file, &op, sizeof(op)) < sizeof(op))
        {
            network_kill_client(n, file);
            return;
        }

        op = ntohs(op);

        switch (op)
        {
        case OP_INSERT:
        {
            Item *item = item_read(file);

            if (!item)
            {
                network_kill_client(n, file);
                return;
            }

            editor_insert_item(e, item);
        }
        break;

        case OP_DELETE:
        {
            int site, clock;

            if ((read(file, &site, sizeof(site)) < sizeof(site)) ||
                (read(file, &clock, sizeof(clock)) < sizeof(clock)))
            {
                network_kill_client(n, file);
                return;
            }

            site = ntohl(site);
            clock = ntohl(clock);
            editor_delete_item(e, site, clock);
        }
        break;

        case OP_BULK_INSERT:
        {
            Items *items = items_read(file);

            if (!items)
            {
                network_kill_client(n, file);
                return;
            }

            // TODO: Better
            for (int i = 0; i < items->size; i++)
                editor_insert_item(e, items->data[i]);

            items->size = 0;
            items_delete(items);
        }
        break;

        case OP_BULK_REQUEST:
            op = htons(OP_BULK_INSERT);

            if ((write(file, &op, sizeof(op)) < sizeof(op)) ||
                !items_write(&e->items, file))
            {
                network_kill_client(n, file);
                return;
            }

            set_status("Sent file to peer");
            break;

        default:
            set_status("Unknown code %d", op);
            break;
        }
    }
}

void network_kill_client(Network *n, int file)
{
    for (int i = 0; i < n->clients.size; i++)
    {
        if (n->clients.data[i] == file)
        {
            clients_remove(&n->clients, i);
            set_status("Client disconnected");
            return;
        }
    }
}

void network_request_file(Network *n)
{
    short op = htons(OP_BULK_REQUEST);
    DIE_IF(write(n->file, &op, sizeof(op)) < sizeof(op), "write");
    set_status("Requested file");
}

void network_insert(Network *n, Item *item)
{
    if (n->mode == CLIENT)
    {
        short op = htons(OP_INSERT);
        DIE_IF(write(n->file, &op, sizeof(op)) < sizeof(op), "write");
        DIE_IF(!item_write(item, n->file), "item_write");
    }
    else if (n->mode == SERVER)
    {
        // Broadcast
        network_insert_broadcast(n, item, -1);
    }
}

void network_delete(Network *n, int site, int clock)
{
    if (n->mode == CLIENT)
    {
        short op = htons(OP_DELETE);
        site = htonl(site);
        clock = htonl(clock);

        DIE_IF(write(n->file, &op, sizeof(op)) < sizeof(op), "write");
        DIE_IF(write(n->file, &site, sizeof(site)) < sizeof(site), "write");
        DIE_IF(write(n->file, &clock, sizeof(clock)) < sizeof(clock), "write");
    }
    else if (n->mode == SERVER)
    {
        // Broadcast
        network_delete_broadcast(n, site, clock, -1);
    }
}

void network_insert_broadcast(Network *n, Item *item, int except)
{
    assert(n->mode == SERVER);
    short op = htons(OP_INSERT);

    Clients to_remove = {0};
    for (int i = 0; i < n->clients.size; i++)
    {
        if (n->clients.data[i] == except)
            break;

        if ((write(n->clients.data[i], &op, sizeof(op)) < sizeof(op)) ||
            !item_write(item, n->clients.data[i]))
            clients_append(&to_remove, n->clients.data[i]);
    }

    // Remove
    for (int i = 0; i < to_remove.size; i++)
        network_kill_client(n, to_remove.data[i]);
    clients_reset(&to_remove);
}

void network_delete_broadcast(Network *n, int site, int clock, int except)
{
    assert(n->mode == SERVER);
    short op = htons(OP_DELETE);
    site = htonl(site);
    clock = htonl(clock);

    Clients to_remove = {0};
    for (int i = 0; i < n->clients.size; i++)
    {
        if (n->clients.data[i] == except)
            break;
        if ((write(n->clients.data[i], &op, sizeof(op)) < sizeof(op)) ||
            (write(n->clients.data[i], &site, sizeof(site)) < sizeof(site)) ||
            (write(n->clients.data[i], &clock, sizeof(clock)) < sizeof(clock)))
            clients_append(&to_remove, n->clients.data[i]);
    }

    // Remove
    for (int i = 0; i < to_remove.size; i++)
        network_kill_client(n, to_remove.data[i]);
    clients_reset(&to_remove);
}
