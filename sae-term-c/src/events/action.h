#ifndef ACTION_H
#define ACTION_H

#include "mode.h"
#include <stddef.h>

/******************************************************************************/
/* Enums                                                                      */
/******************************************************************************/

enum ActionType
{
    ACTION_TYPE_CURSOR_STATUS,
    ACTION_TYPE_CHANGE_MODE,
    ACTION_TYPE_QUIT,
};

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef enum ActionType ActionType;

typedef struct CursorStatusData CursorStatusData;
typedef struct ChangeModeData   ChangeModeData;
typedef struct Action           Action;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct CursorStatusData
{
    size_t x;
    size_t y;
};

struct Action
{
    ActionType type;
    union {
        CursorStatusData cursor_status;
        EditMode         change_mode;
    } data;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

Action *action_create(ActionType type);

#endif // ACTION_H
