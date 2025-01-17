#ifndef BUFFER_COMPONENT_H
#define BUFFER_COMPONENT_H

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
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void buffer_component_init(BufferComponent *self);
void buffer_component_fini(BufferComponent *self);

#endif // BUFFER_COMPONENT_H
