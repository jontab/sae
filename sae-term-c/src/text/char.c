#include "char.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define COMPARE(A, B) (((A) > (B)) - ((A) < (B)))

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int char_index_compare(const CharIndex *a, const CharIndex *b)
{
    int x = COMPARE(a->parent_clock, b->parent_clock);
    if (x)
    {
        return x;
    }

    x = COMPARE(a->parent_site, b->parent_site);
    if (x)
    {
        return x;
    }

    x = COMPARE(a->clock, b->clock);
    if (x)
    {
        return x;
    }

    return COMPARE(a->site, b->site);
}

int char_index_search(const CharIndex *array, int size, const CharIndex *what)
{
    int l = 0;
    int r = size;
    while (l < r)
    {
        int mid = (l + r) / 2;
        int cmp = char_index_compare(&array[mid], what);
        if (cmp < 0)
        {
            l = mid + 1;
        }
        else
        {
            r = mid;
        }
    }

    return l;
}