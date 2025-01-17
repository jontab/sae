#ifndef COMPONENT_H
#define COMPONENT_H

#include "../events/action.h"
#include "../events/event.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define component_handle_event(P, E)  ((Component *)(P))->handle_event((Component *)(P), (E))
#define component_handle_action(P, E) ((Component *)(P))->handle_action((Component *)(P), (E))
#define component_draw(P)             ((Component *)(P))->draw((Component *)(P))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct Component Component;

typedef void (*ComponentHandleEvent)(Component *self, Event *event);
typedef void (*ComponentHandleAction)(Component *self, Action *action);
typedef void (*ComponentDraw)(Component *self);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct Component
{
    ComponentHandleEvent  handle_event;
    ComponentHandleAction handle_action;
    ComponentDraw         draw;
};

#endif // COMPONENT_H
