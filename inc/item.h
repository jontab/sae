#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>

typedef struct Item
{
    int  site;
    int  clock;
    int  parent_site;
    int  parent_clock;
    char value;
    bool deleted;
} Item;

bool  item_write (Item *item, int file);
Item *item_read  (int file);

#endif /* ITEM_H */
