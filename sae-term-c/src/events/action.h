#ifndef ACTION_H
#define ACTION_H

#include <stddef.h>

/******************************************************************************/
/* Enums                                                                      */
/******************************************************************************/

enum ActionType
{
    ACTION_TYPE_CURSOR_STATUS,
};

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef enum ActionType ActionType;

typedef struct CursorStatusData CursorStatusData;
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
    } data;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

Action *action_create(ActionType type);

#endif // ACTION_H
