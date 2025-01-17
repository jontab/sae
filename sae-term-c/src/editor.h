#ifndef EDITOR_H
#define EDITOR_H

#include "components/buffer.h"
#include "components/status.h"
#include "events/queue.h"
#include <stdbool.h>
#include <stdio.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct Editor Editor;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct Editor
{
    BufferComponent buffer_component;
    StatusComponent status_component;
    Queue           event_tx;  // Transmit event.
    Queue           action_tx; // Transmit action.
    bool            is_running;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void editor_init(Editor *self);
void editor_fini(Editor *self);
void editor_exec(Editor *self);

#endif // EDITOR_H
