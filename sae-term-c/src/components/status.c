#include "status.h"
#include "../check.h"
#include "termbox2.h"

static void status_component_handle_event(StatusComponent *self, Event *event);
static void status_component_handle_event_key(StatusComponent *self, Event *event);
static void status_component_handle_action(StatusComponent *self, Action *action);
static void status_component_draw(StatusComponent *self);
static void status_component_draw_normal(StatusComponent *self);
// static void status_component_draw_focus(StatusComponent *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void status_component_init(StatusComponent *self, Queue *action_tx)
{
    memset(self, 0, sizeof(StatusComponent));
    self->base = (Component){
        .handle_event = (ComponentHandleEvent)(status_component_handle_event),
        .handle_action = (ComponentHandleAction)(status_component_handle_action),
        .draw = (ComponentDraw)(status_component_draw),
    };
    self->action_tx = action_tx;
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
    switch (event->type)
    {
    case EVENT_TYPE_KEY:
        status_component_handle_event_key(self, event);
        break;
    default:
        break;
    }
}

void status_component_handle_event_key(StatusComponent *self, Event *event)
{
    (void)(self);
    (void)(event);
}

void status_component_handle_action(StatusComponent *self, Action *action)
{
    switch (action->type)
    {
    case ACTION_TYPE_CURSOR_STATUS:
        self->known_cursor_y = action->data.cursor_status.y;
        self->known_cursor_x = action->data.cursor_status.x;
        break;
    default:
        break;
    }
}

void status_component_draw(StatusComponent *self)
{
    status_component_draw_normal(self);
}

void status_component_draw_normal(StatusComponent *self)
{
    int height = tb_height();
    int width = tb_width();
    WARN_CHECK_INT(height, >= 0);
    WARN_CHECK_INT(width, >= 0);

    //
    char status_l[BUFSIZ];
    char status_r[BUFSIZ];
    char status[BUFSIZ]; // Result.
    snprintf(status_l, sizeof(status_l), "untitled");
    snprintf(status_r, sizeof(status_r), "%zu, %zu", self->known_cursor_y, self->known_cursor_x);
    if ((int)(strlen(status_l)) < width)
    {
        int justify = width - (int)(strlen(status_l));
        snprintf(status, sizeof(status), "%s%*s", status_l, justify, status_r);
    }
    else
    {
        snprintf(status, sizeof(status), "%s", status_l);
    }

    for (int view_x = 0; view_x < width; view_x++)
    {
        int view_y = height - 1;
        WARN_CHECK_INT(tb_set_cell(view_x, view_y, status[view_x], TB_DEFAULT, TB_WHITE), == TB_OK);
    }
}

// void status_component_draw_focus(StatusComponent *self)
// {
//     int height = tb_height();
//     int width = tb_width();
//     WARN_CHECK_INT(height, >= 0);
//     WARN_CHECK_INT(width, >= 0);
// }
