#ifndef BUFFER_H
#define BUFFER_H

#include "char.h"
#include "list.h"
#include <stdbool.h>
#include <stddef.h>

struct Line;

LIST_GENERATE_HEADER(LineList, line_list, struct Line)
LIST_GENERATE_HEADER(CharList, char_list, char)

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct Line   Line;
typedef struct Buffer Buffer;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct Line
{
    size_t begin;
    size_t end;
};

struct Buffer
{
    LineList lines;
    CharList chars;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void   buffer_init(Buffer *self);
void   buffer_fini(Buffer *self);
void   buffer_insert(Buffer *self, size_t y, size_t x, char ch);
void   buffer_remove(Buffer *self, size_t y, size_t x);
void   buffer_line_content(Buffer *self, size_t at, bool (*iter)(char ch, void *user), void *user);
size_t buffer_line_length(Buffer *self, size_t at);
size_t buffer_line_count(Buffer *self);

#endif // BUFFER_H
