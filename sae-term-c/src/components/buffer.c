#include "buffer.h"
#include "../check.h"
#include "../events/queue.h"
#include "termbox2.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

struct DrawLineContext;

static void buffer_component_handle_event(BufferComponent *self, Event *event);
static void buffer_component_handle_event_key(BufferComponent *self, KeyData *data);
static void buffer_component_handle_event_key_navigate(BufferComponent *self, KeyData *data);
static void buffer_component_handle_event_key_modify(BufferComponent *self, KeyData *data);
static void buffer_component_handle_action(BufferComponent *self, Action *action);
static void buffer_component_draw(BufferComponent *self);
static bool buffer_component_draw_line(char ch, void *data);
static bool buffer_component_draw_line_put(char ch, struct DrawLineContext *context);
static void buffer_component_draw_cursor(BufferComponent *self, Point *anchor, Point *size, Point *cursor);
static bool buffer_component_draw_cursor_callback(char ch, void *data);
static void buffer_component_area(BufferComponent *self, Point *anchor, Point *size);
static void buffer_component_handle_up(BufferComponent *self);
static void buffer_component_handle_down(BufferComponent *self);
static void buffer_component_handle_left(BufferComponent *self);
static void buffer_component_handle_right(BufferComponent *self);
static void buffer_component_update_cursor_status(BufferComponent *self);

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct DrawLineContext   DrawLineContext;
typedef struct DrawCursorContext DrawCursorContext;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct DrawLineContext
{
    Point *view_anchor;
    Point *view_size;
    size_t view_dy;
    size_t view_dx;
    size_t view_offset_remaining;
};

struct DrawCursorContext
{
    Point *view_anchor;
    Point *view_size;
    size_t view_dx;
    size_t x;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void buffer_component_init(BufferComponent *self, Queue *action_tx)
{
    memset(self, 0, sizeof(BufferComponent));
    self->base = (Component){
        .handle_event = (ComponentHandleEvent)(buffer_component_handle_event),
        .handle_action = (ComponentHandleAction)(buffer_component_handle_action),
        .draw = (ComponentDraw)(buffer_component_draw),
    };
    self->action_tx = action_tx;
    buffer_init(&self->buffer);
}

void buffer_component_fini(BufferComponent *self)
{
    buffer_fini(&self->buffer);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void buffer_component_handle_event(BufferComponent *self, Event *event)
{
    switch (event->type)
    {
    case EVENT_TYPE_KEY:
        buffer_component_handle_event_key(self, &event->data.key);
        break;
    default:
        break;
    }
}

void buffer_component_handle_event_key(BufferComponent *self, KeyData *data)
{
    switch (self->edit_mode)
    {
    case EDIT_MODE_INSERT:
        buffer_component_handle_event_key_modify(self, data);
        // Fall into the next case.
    case EDIT_MODE_NORMAL:
        buffer_component_handle_event_key_navigate(self, data);
        break;
    default:
        break;
    }
}

void buffer_component_handle_event_key_navigate(BufferComponent *self, KeyData *data)
{
    if (data->ch == 'i')
    {
        Action *action = action_create(ACTION_TYPE_CHANGE_MODE);
        action->data.change_mode = EDIT_MODE_INSERT;
        queue_push(self->action_tx, action);
    }

    if ((self->edit_mode == EDIT_MODE_NORMAL) && (data->ch == ':'))
    {
        Action *action = action_create(ACTION_TYPE_CHANGE_MODE);
        action->data.change_mode = EDIT_MODE_COMMAND;
        queue_push(self->action_tx, action);
    }

    switch (data->key)
    {
    case TB_KEY_ARROW_UP:
        buffer_component_handle_up(self);
        break;
    case TB_KEY_ARROW_DOWN:
        buffer_component_handle_down(self);
        break;
    case TB_KEY_ARROW_LEFT:
        buffer_component_handle_left(self);
        break;
    case TB_KEY_ARROW_RIGHT:
        buffer_component_handle_right(self);
        break;
    default:
        break;
    }
}

void buffer_component_handle_event_key_modify(BufferComponent *self, KeyData *data)
{
    if (!data->mod && !data->key)
    {
        buffer_insert(&self->buffer, self->cursor.y, self->cursor.x, data->ch);
        buffer_component_handle_right(self);
    }

    switch (data->key)
    {
    case TB_KEY_BACKSPACE:
    case TB_KEY_BACKSPACE2:
        buffer_component_handle_left(self);
        buffer_remove(&self->buffer, self->cursor.y, self->cursor.x);
        break;
    case TB_KEY_TAB:
        buffer_insert(&self->buffer, self->cursor.y, self->cursor.x, '\t');
        buffer_component_handle_right(self);
        break;
    case TB_KEY_ENTER:
        buffer_insert(&self->buffer, self->cursor.y, self->cursor.x, '\n');
        self->cursor.y++;
        self->cursor_hint = self->cursor.x = 0;
        buffer_component_update_cursor_status(self);
        break;
    case TB_KEY_ESC: {
        Action *action = action_create(ACTION_TYPE_CHANGE_MODE);
        action->data.change_mode = EDIT_MODE_NORMAL;
        queue_push(self->action_tx, action);
        break;
    }
    default:
        break;
    }
}

void buffer_component_handle_action(BufferComponent *self, Action *action)
{
    switch (action->type)
    {
    case ACTION_TYPE_CHANGE_MODE:
        self->edit_mode = action->data.change_mode;
        break;
    default:
        break;
    }
}

void buffer_component_draw(BufferComponent *self)
{
    Point view_anchor;
    Point view_size;
    buffer_component_area(self, &view_anchor, &view_size);
    for (size_t view_dy = 0; view_dy < view_size.y; view_dy++)
    {
        DrawLineContext context = {
            .view_anchor = &view_anchor,
            .view_size = &view_size,
            .view_dy = view_dy,
            .view_dx = 0,
            .view_offset_remaining = self->camera.x,
        };
        size_t line_index = self->camera.y + view_dy;
        buffer_line_content(&self->buffer, line_index, buffer_component_draw_line, &context);

        //
        for (; context.view_dx < view_size.x; context.view_dx++)
        {
            size_t view_y = context.view_dy + view_anchor.y;
            size_t view_x = context.view_dx + view_anchor.x;
            WARN_CHECK_INT(tb_set_cell(view_x, view_y, ' ', TB_WHITE, TB_DEFAULT), == TB_OK);
        }
    }

    switch (self->edit_mode)
    {
    case EDIT_MODE_NORMAL:
    case EDIT_MODE_INSERT:
        buffer_component_draw_cursor(self, &view_anchor, &view_size, &self->cursor);
        break;
    default:
        break;
    }
}

bool buffer_component_draw_line(char ch, void *data)
{
    DrawLineContext *context = (DrawLineContext *)(data);
    if (ch == '\t')
    {
        int tabs = 4 - (context->view_dx % 4);
        for (int i = 0; i < tabs; i++)
        {
            if (!buffer_component_draw_line_put(' ', context))
            {
                return false;
            }
        }
    }
    else
    {
        if (!buffer_component_draw_line_put(ch, context))
        {
            return false;
        }
    }

    return true;
}

bool buffer_component_draw_line_put(char ch, DrawLineContext *context)
{
    bool is_before = context->view_offset_remaining > 0;
    bool is_after = context->view_dx >= context->view_size->x;
    if (!is_before && !is_after)
    {
        size_t view_y = context->view_dy + context->view_anchor->y;
        size_t view_x = context->view_dx + context->view_anchor->x;
        WARN_CHECK_INT(tb_set_cell(view_x, view_y, ch, TB_WHITE, TB_DEFAULT), == TB_OK);
        context->view_dx++;
        return true;
    }
    else if (is_before)
    {
        context->view_offset_remaining--;
        return true;
    }
    else
    {
        return false;
    }
}

void buffer_component_draw_cursor(BufferComponent *self, Point *anchor, Point *size, Point *cursor)
{
    DrawCursorContext context = {
        .view_anchor = anchor,
        .view_size = size,
        .view_dx = 0,
        .x = cursor->x,
    };
    buffer_line_content(&self->buffer, cursor->y, buffer_component_draw_cursor_callback, &context);

    //
    size_t view_dy = cursor->y - self->camera.y;
    size_t view_dx = context.view_dx - self->camera.x;
    if ((view_dy < size->y) && (view_dx < size->x))
    {
        size_t view_y = view_dy + anchor->y;
        size_t view_x = view_dx + anchor->x;
        WARN_CHECK_INT(tb_set_cursor(view_x, view_y), == TB_OK);
        DEBUG("tb_set_cursor(%d, %d)", view_x, view_y);
    }
}

bool buffer_component_draw_cursor_callback(char ch, void *data)
{
    DrawCursorContext *context = (DrawCursorContext *)(data);
    if (context->x)
    {
        if (ch == '\t')
        {
            int tabs = 4 - (context->view_dx % 4);
            context->view_dx += tabs;
        }
        else
        {
            context->view_dx++;
        }

        context->x--;
        return true;
    }
    else
    {
        return false;
    }
}

void buffer_component_area(BufferComponent *self, Point *anchor, Point *size)
{
    anchor->x = 0;
    anchor->y = 0;
    size->x = tb_width();
    size->y = tb_height() - 1;
    WARN_CHECK_INT(size->x, >= 0);
    WARN_CHECK_INT(size->y, >= 0);
}

void buffer_component_handle_up(BufferComponent *self)
{
    if (self->cursor.y > 0)
    {
        self->cursor.y--;
        size_t line_length = buffer_line_length(&self->buffer, self->cursor.y);
        self->cursor.x = MIN(self->cursor_hint, line_length);
    }

    buffer_component_update_cursor_status(self);
}

void buffer_component_handle_down(BufferComponent *self)
{
    if (self->cursor.y < buffer_line_count(&self->buffer) - 1)
    {
        self->cursor.y++;
        size_t line_length = buffer_line_length(&self->buffer, self->cursor.y);
        self->cursor.x = MIN(self->cursor_hint, line_length);
    }

    buffer_component_update_cursor_status(self);
}

void buffer_component_handle_left(BufferComponent *self)
{
    if (self->cursor.x > 0)
    {
        self->cursor_hint = (--self->cursor.x);
    }
    else if (self->cursor.y > 0)
    {
        self->cursor.y--;
        size_t line_length = buffer_line_length(&self->buffer, self->cursor.y);
        self->cursor_hint = self->cursor.x = line_length;
    }

    buffer_component_update_cursor_status(self);
}

void buffer_component_handle_right(BufferComponent *self)
{
    size_t line_length = buffer_line_length(&self->buffer, self->cursor.y);
    if (self->cursor.x < line_length)
    {
        self->cursor_hint = (++self->cursor.x);
    }
    else if (self->cursor.y < buffer_line_count(&self->buffer) - 1)
    {
        self->cursor.y++;
        self->cursor_hint = self->cursor.x = 0;
    }

    buffer_component_update_cursor_status(self);
}

void buffer_component_update_cursor_status(BufferComponent *self)
{
    Action *action = action_create(ACTION_TYPE_CURSOR_STATUS);
    action->data.cursor_status.y = self->cursor.y;
    action->data.cursor_status.x = self->cursor.x;
    queue_push(self->action_tx, action);
}
