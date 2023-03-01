#ifndef ITEMS_H
#define ITEMS_H

#include "item.h"
#include "list.header.h"

#include <stdbool.h>

LIST_GENERATE_HEADER(Item *, Items, items)

int    items_insort (Items *items, Item *item);
int    items_find   (Items *items, int site, int clock);
bool   items_write  (Items *items, int file);
Items *items_read   (int file);

#endif /* ITEMS_H */
