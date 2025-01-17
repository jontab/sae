#ifndef STATUS_COMPONENT_H
#define STATUS_COMPONENT_H

#include "component.h"

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
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void status_component_init(StatusComponent *self);
void status_component_fini(StatusComponent *self);

#endif // STATUS_COMPONENT_H
