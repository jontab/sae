#include "buffer.h"

static void buffer_component_handle_event(BufferComponent *self, Event *event);
static void buffer_component_handle_action(BufferComponent *self, Action *action);
static void buffer_component_draw(BufferComponent *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void buffer_component_init(BufferComponent *self)
{
    self->base = (Component){
        .handle_event = (ComponentHandleEvent)(buffer_component_handle_event),
        .handle_action = (ComponentHandleAction)(buffer_component_handle_action),
        .draw = (ComponentDraw)(buffer_component_draw),
    };
}

void buffer_component_fini(BufferComponent *self)
{
    (void)(self);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void buffer_component_handle_event(BufferComponent *self, Event *event)
{
    (void)(self);
    (void)(event);
}

void buffer_component_handle_action(BufferComponent *self, Action *action)
{
    (void)(self);
    (void)(action);
}

void buffer_component_draw(BufferComponent *self)
{
    (void)(self);
}
