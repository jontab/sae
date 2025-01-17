#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct QueueNode QueueNode;
typedef struct Queue     Queue;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct QueueNode
{
    QueueNode *next;
    void      *data;
};

struct Queue
{
    QueueNode *head;
    QueueNode *tail;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void queue_init(Queue *self);
void queue_fini(Queue *self);
void queue_push(Queue *self, void *data);
bool queue_pop(Queue *self, void **out);

#endif // QUEUE_H
