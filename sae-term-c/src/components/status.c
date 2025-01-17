#include "status.h"

static void status_component_handle_event(StatusComponent *self, Event *event);
static void status_component_handle_action(StatusComponent *self, Action *action);
static void status_component_draw(StatusComponent *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void status_component_init(StatusComponent *self)
{
    self->base = (Component){
        .handle_event = (ComponentHandleEvent)(status_component_handle_event),
        .handle_action = (ComponentHandleAction)(status_component_handle_action),
        .draw = (ComponentDraw)(status_component_draw),
    };
}

void status_component_fini(StatusComponent *self)
{
    (void)(self);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void status_component_handle_event(StatusComponent *self, Event *event)
{
    (void)(self);
    (void)(event);
}

void status_component_handle_action(StatusComponent *self, Action *action)
{
    (void)(self);
    (void)(action);
}

void status_component_draw(StatusComponent *self)
{
    (void)(self);
}
