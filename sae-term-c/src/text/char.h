#ifndef CHAR_H
#define CHAR_H

#include <stddef.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct CharIndex CharIndex;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct CharIndex
{
    int parent_clock;
    int parent_site;
    int clock;
    int site;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int char_index_compare(const CharIndex *a, const CharIndex *b);
int char_index_search(const CharIndex *array, int size, const CharIndex *what);

#endif // CHAR_H
