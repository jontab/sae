#include "queue.h"
#include "../check.h"
#include <stdlib.h>
#include <string.h>

static QueueNode *queue_node_create(void *data);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void queue_init(Queue *self)
{
    memset(self, 0, sizeof(Queue));
}

void queue_fini(Queue *self)
{
    for (QueueNode *curr = self->head; curr;)
    {
        QueueNode *next = curr->next;
        free(curr);
        curr = next;
    }
}

void queue_push(Queue *self, void *data)
{
    if (self->tail)
    {
        self->tail = self->tail->next = queue_node_create(data);
    }
    else
    {
        self->tail = self->head = queue_node_create(data);
    }
}

bool queue_pop(Queue *self, void **out)
{
    if (self->head)
    {
        QueueNode *node = self->head;
        void      *data = node->data;
        if (self->head == self->tail)
        {
            self->head = self->tail = NULL;
        }
        else
        {
            self->head = self->head->next;
        }

        *out = data;
        free(node);
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

QueueNode *queue_node_create(void *data)
{
    QueueNode *node = calloc(1, sizeof(QueueNode));
    CHECK(node);
    node->data = data;
    return node;
}
