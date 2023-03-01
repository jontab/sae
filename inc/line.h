#ifndef LINE_H
#define LINE_H

#include "items.h"

typedef struct Line
{
    int begin;
    int end;
} Line;

int line_offset_calculate (Line *line, Items *items, int n);
int line_offset_add       (Line *line, Items *items, int n);

#endif /* LINE_H */
