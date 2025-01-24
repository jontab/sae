#include "action.h"
#include "../check.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

Action *action_create(ActionType type)
{
    Action *self = calloc(1, sizeof(Action));
    CHECK(self);
    self->type = type;
    return self;
}
