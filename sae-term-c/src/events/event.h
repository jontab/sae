#ifndef EVENT_H
#define EVENT_H

/******************************************************************************/
/* Enums                                                                      */
/******************************************************************************/

enum EventType
{
    EVENT_TYPE_TICK = 0,
    EVENT_TYPE_KEY,
};

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef enum EventType EventType;

typedef struct KeyData KeyData;
typedef struct Event   Event;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct KeyData
{
    int ch;
    int mod; // TB_MOD_*.
    int key; // TB_KEY_*.
};

struct Event
{
    EventType type;
    union {
        KeyData key;
    } data;
};

#endif // EVENT_H
