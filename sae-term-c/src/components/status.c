#include "status.h"
#include "../check.h"
#include "termbox2.h"

static void status_component_handle_event(StatusComponent *self, Event *event);
static void status_component_handle_event_key(StatusComponent *self, KeyData *data);
static void status_component_handle_action(StatusComponent *self, Action *action);
static void status_component_draw(StatusComponent *self);
static void status_component_draw_normal(StatusComponent *self);
static void status_component_draw_command(StatusComponent *self);
static void status_component_adjust_offset_left(StatusComponent *self);
static void status_component_adjust_offset_right(StatusComponent *self, int width);

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
    if (self->command_buffer)
    {
        free(self->command_buffer);
        self->command_buffer = NULL;
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void status_component_handle_event(StatusComponent *self, Event *event)
{
    switch (event->type)
    {
    case EVENT_TYPE_KEY:
        status_component_handle_event_key(self, &event->data.key);
        break;
    default:
        break;
    }
}

void status_component_handle_event_key(StatusComponent *self, KeyData *data)
{
    if (self->edit_mode == EDIT_MODE_COMMAND)
    {
        if (isprint(data->ch))
        {
            size_t size = strlen(self->command_buffer) + 1;
            self->command_buffer = realloc(self->command_buffer, size + 1);
            CHECK(self->command_buffer);
            memmove(&self->command_buffer[size + 1], &self->command_buffer[size], size - self->command_x);
            self->command_buffer[(self->command_x++)] = data->ch;
        }

        switch (data->key)
        {
        case TB_KEY_BACKSPACE:
        case TB_KEY_BACKSPACE2:
            if (self->command_x > 0)
            {
                self->command_x--;
            }
            // Fall into the next case.
        case TB_KEY_DELETE: {
            size_t length = strlen(self->command_buffer); // String.
            size_t size = length + 1;                     // Buffer (with '\0').
            if (self->command_x < length)
            {
                memmove(&self->command_buffer[self->command_x], &self->command_buffer[self->command_x + 1],
                        size - self->command_x);
            }
            break;
        }

        case TB_KEY_ENTER: {
            if ((strlen(self->command_buffer) > 1) && (self->command_buffer[1] == 'q'))
            {
                Action *action = action_create(ACTION_TYPE_QUIT);
                queue_push(self->action_tx, action);
            }
            // Fall into the next case.
        }

        case TB_KEY_ESC: {
            Action *action = action_create(ACTION_TYPE_CHANGE_MODE);
            action->data.change_mode = EDIT_MODE_NORMAL;
            queue_push(self->action_tx, action);
            break;
        }

        case TB_KEY_ARROW_LEFT:
            if (self->command_x)
            {
                self->command_x--;
            }
            break;

        case TB_KEY_ARROW_RIGHT:
            if (self->command_x < strlen(self->command_buffer))
            {
                self->command_x++;
            }
            break;
        }
    }
}

void status_component_handle_action(StatusComponent *self, Action *action)
{
    switch (action->type)
    {
    case ACTION_TYPE_CURSOR_STATUS:
        self->known_cursor_y = action->data.cursor_status.y;
        self->known_cursor_x = action->data.cursor_status.x;
        break;
    case ACTION_TYPE_CHANGE_MODE: {
        self->edit_mode = action->data.change_mode;
        if (action->data.change_mode == EDIT_MODE_COMMAND)
        {
            if (self->command_buffer)
            {
                free(self->command_buffer);
                self->command_buffer = NULL;
            }

            self->command_x_offset = 0;
            self->command_x = 1;
            self->command_buffer = strdup(":");
            CHECK(self->command_buffer);
        }
        break;
    }
    default:
        break;
    }
}

void status_component_draw(StatusComponent *self)
{
    if (self->edit_mode == EDIT_MODE_COMMAND)
    {
        status_component_draw_command(self);
    }
    else
    {
        status_component_draw_normal(self);
    }
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
    snprintf(status_l, sizeof(status_l), (self->edit_mode == EDIT_MODE_NORMAL) ? "-- NORMAL --" : "-- INSERT --");
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

void status_component_draw_command(StatusComponent *self)
{
    int height = tb_height();
    int width = tb_width();
    WARN_CHECK_INT(height, >= 0);
    WARN_CHECK_INT(width, >= 0);

    //
    status_component_adjust_offset_left(self);
    status_component_adjust_offset_right(self, width);

    //
    for (int view_x = 0; view_x < width; view_x++)
    {
        int  view_y = height - 1;
        int  x = view_x + (int)(self->command_x_offset);
        char c = (self->command_buffer && (x < strlen(self->command_buffer))) ? self->command_buffer[x] : ' ';
        WARN_CHECK_INT(tb_set_cell(view_x, view_y, c, TB_WHITE, TB_DEFAULT), == TB_OK);

        // Cursor.
        if (x == self->command_x)
        {
            WARN_CHECK_INT(tb_set_cursor(view_x, view_y), == TB_OK);
        }
    }
}

void status_component_adjust_offset_left(StatusComponent *self)
{
    while (self->command_x_offset > self->command_x)
    {
        self->command_x_offset--;
    }
}

void status_component_adjust_offset_right(StatusComponent *self, int width)
{
    while ((self->command_x_offset <= self->command_x) && ((self->command_x - self->command_x_offset) >= width))
    {
        self->command_x_offset++;
    }
}
