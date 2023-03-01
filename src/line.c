#include "line.h"

int line_offset_calculate(Line *line, Items *items, int n)
{
    // Calculate the number of visible characters between the beginning of
    // a line and some later index.
    int count = 0;
    for (int i = line->begin; i < n; i++)
        if (!items->data[i]->deleted)
            count++;
    return count;
}

int line_offset_add(Line *line, Items *items, int n)
{
    for (int i = line->begin; i < line->end; i++)
        if (!items->data[i]->deleted)
            if ((n--) == 0)
                return i;
    return line->end;
}
