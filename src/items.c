#include "items.h"
#include "list.source.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

LIST_GENERATE_SOURCE(Item *, Items, items, free)

int items_insort(Items *items, Item *item)
{
    int par = items_find(items, item->parent_site, item->parent_clock);
    int i;

    for (i = par + 1; i < items->size; i++)
    {
        Item *o = items->data[i];
        if (item->clock > o->clock)
            break;

        int opar = items_find(items, o->parent_site, o->parent_clock);
        if ((opar < par) ||
            ((opar == par) &&
             (item->clock == o->clock) &&
             (item->site < o->site)))
            break;
    }

    items_insert(items, item, i);
    return i;
}

int items_find(Items *items, int site, int clock)
{
    for (int i = 0; i < items->size; i++)
        if ((items->data[i]->site == site) && (items->data[i]->clock == clock))
            return i;
    return -1;
}

bool items_write(Items *items, int file)
{
    int amount = htonl(items->size);
    if (write(file, &amount, sizeof(amount)) < sizeof(amount))
        return false;

    for (int i = 0; i < items->size; i++)
        if (!item_write(items->data[i], file))
            return false;

    return true;
}

Items *items_read(int file)
{
    int amount;
    if (read(file, &amount, sizeof(amount)) < sizeof(amount))
        return NULL;
    amount = ntohl(amount);

    Items *items = items_create(amount);
    for (int i = 0; i < amount; i++)
    {
        Item *item = item_read(file);
        if (!item)
        {
            items_delete(items);
            return NULL;
        }

        items_append(items, item);
    }

    return items;
}
