#include "item.h"
#include "core.h"

#include <arpa/inet.h>
#include <unistd.h>

bool item_write(Item *item, int file)
{
    int site = htonl(item->site);
    int clock = htonl(item->clock);
    int parent_site = htonl(item->parent_site);
    int parent_clock = htonl(item->parent_clock);
    short value = htons(item->value);
    short deleted = htons(item->deleted);

    if ((write(file, &site, sizeof(site)) < sizeof(site)) ||
        (write(file, &clock, sizeof(clock)) < sizeof(clock)) ||
        (write(file, &parent_site, sizeof(parent_site)) < sizeof(parent_site)) ||
        (write(file, &parent_clock, sizeof(parent_clock)) < sizeof(parent_clock)) ||
        (write(file, &value, sizeof(value)) < sizeof(value)) ||
        (write(file, &deleted, sizeof(deleted)) < sizeof(deleted)))
        return false;

    return true;
}

Item *item_read(int file)
{
    int site, clock, parent_site, parent_clock;
    short value, deleted;

    if ((read(file, &site, sizeof(site)) < sizeof(site)) ||
        (read(file, &clock, sizeof(clock)) < sizeof(clock)) ||
        (read(file, &parent_site, sizeof(parent_site)) < sizeof(parent_site)) ||
        (read(file, &parent_clock, sizeof(parent_clock)) < sizeof(parent_clock)) ||
        (read(file, &value, sizeof(value)) < sizeof(value)) ||
        (read(file, &deleted, sizeof(deleted)) < sizeof(deleted)))
        return NULL;

    Item *item = ecalloc(1, sizeof(Item));
    item->site = ntohl(site);
    item->clock = ntohl(clock);
    item->parent_site = ntohl(parent_site);
    item->parent_clock = ntohl(parent_clock);
    item->value = (char)ntohs(value);
    item->deleted = (bool)ntohs(deleted);
    return item;
}
