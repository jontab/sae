#ifndef BUFFER_COMPONENT_H
#define BUFFER_COMPONENT_H

#include "../events/queue.h"
#include "../objects/point.h"
#include "../text/buffer.h"
#include "component.h"

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct BufferComponent BufferComponent;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct BufferComponent
{
    Component base;
    Queue    *action_tx;
    Buffer    buffer;
    Point     camera;
    Point     cursor;
    size_t    cursor_hint;
    EditMode  edit_mode;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void buffer_component_init(BufferComponent *self, Queue *action_tx);
void buffer_component_fini(BufferComponent *self);

#endif // BUFFER_COMPONENT_H
