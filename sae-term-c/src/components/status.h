#ifndef STATUS_COMPONENT_H
#define STATUS_COMPONENT_H

#include "../events/queue.h"
#include "component.h"
#include <stdbool.h>
#include <stdio.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct StatusComponent StatusComponent;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct StatusComponent
{
    Component base;
    Queue    *action_tx;
    size_t    known_cursor_x;
    size_t    known_cursor_y;
    EditMode  edit_mode;
    size_t    command_x_offset; // `EDIT_MODE_COMMAND`.
    size_t    command_x;        // `EDIT_MODE_COMMAND`.
    char     *command_buffer;   // `EDIT_MODE_COMMAND`.
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void status_component_init(StatusComponent *self, Queue *action_tx);
void status_component_fini(StatusComponent *self);

#endif // STATUS_COMPONENT_H
