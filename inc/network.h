#ifndef NETWORK_H
#define NETWORK_H

#include "clients.h"
#include "item.h"

#include <stdbool.h>
#include <sys/select.h>

typedef struct Editor Editor;

typedef enum NetworkCode
{
    OP_INSERT,
    OP_DELETE,
    OP_BULK_INSERT,
    OP_BULK_REQUEST,
} NetworkCode;

typedef enum NetworkMode
{
    OFF,
    CLIENT,
    SERVER,
} NetworkMode;

typedef struct Network
{
    Clients     clients;
    int         file;
    NetworkMode mode;
} Network;

bool network_start            (Network *n, const char *host, const char *port, NetworkMode mode);
int  network_start_client     (const char *host, const char *port);
int  network_start_server     (const char *host, const char *port);
void network_stop             (Network *n);

void network_add_files_set    (Network *n, fd_set *set, int *max);
void network_handle           (Network *n, int file, Editor *e);
void network_kill_client      (Network *n, int file);

void network_request_file     (Network *n);

void network_insert           (Network *n, Item *item);
void network_delete           (Network *n, int site, int clock);
void network_insert_broadcast (Network *n, Item *item, int except);
void network_delete_broadcast (Network *n, int site, int clock, int except);

#endif /* NETWORK_H */
