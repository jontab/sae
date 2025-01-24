#include "buffer.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

LIST_GENERATE_SOURCE(LineList, line_list, Line)
LIST_GENERATE_SOURCE(CharList, char_list, char)

static void buffer_calculate_lines(Buffer *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void buffer_init(Buffer *self)
{
    line_list_init(&self->lines);
    char_list_init(&self->chars);
    buffer_calculate_lines(self);
}

void buffer_fini(Buffer *self)
{
    line_list_fini(&self->lines);
    char_list_fini(&self->chars);
}

void buffer_insert(Buffer *self, size_t y, size_t x, char ch)
{
    if (y < self->lines.size)
    {
        Line  *line = &self->lines.data[y];
        size_t line_length = line->end - line->begin;
        size_t line_offset = MIN(x, line_length);
        char_list_insert(&self->chars, ch, line->begin + line_offset);
        buffer_calculate_lines(self);
    }
}

void buffer_remove(Buffer *self, size_t y, size_t x)
{
    if (y < self->lines.size)
    {
        Line  *line = &self->lines.data[y];
        size_t line_length = line->end - line->begin;
        size_t line_offset = MIN(x, line_length);
        if (line->begin + line_offset < self->chars.size)
        {
            char_list_remove(&self->chars, line->begin + line_offset);
            buffer_calculate_lines(self);
        }
    }
}

void buffer_line_content(Buffer *self, size_t at, bool (*iter)(char ch, void *user), void *user)
{
    if (at < self->lines.size)
    {
        Line *line = &self->lines.data[at];
        for (size_t i = line->begin; i < line->end; i++)
        {
            if (!iter(self->chars.data[i], user))
            {
                return;
            }
        }
    }
}

size_t buffer_line_length(Buffer *self, size_t at)
{
    if (at < self->lines.size)
    {
        Line *line = &self->lines.data[at];
        return line->end - line->begin;
    }
    else
    {
        return 0;
    }
}

size_t buffer_line_count(Buffer *self)
{
    return self->lines.size;
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void buffer_calculate_lines(Buffer *self)
{
    line_list_clear(&self->lines);

    //
    size_t begin = 0;
    for (size_t i = 0; i < self->chars.size; i++)
    {
        if (self->chars.data[i] == '\n')
        {
            line_list_append(&self->lines, (Line){.begin = begin, .end = i});
            begin = i + 1;
        }
    }

    line_list_append(&self->lines, (Line){.begin = begin, .end = self->chars.size});
}
