#include "editor.h"
#include "check.h"
#include "events/action.h"
#include "termbox2.h"
#include "view.h"
#include <stdlib.h>
#include <string.h>

static void editor_handle_events(Editor *self);
static void editor_handle_events_key(Editor *self, KeyData *data);
static void editor_handle_actions(Editor *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void editor_init(Editor *self)
{
    memset(self, 0, sizeof(Editor));
    queue_init(&self->event_tx);
    queue_init(&self->action_tx);
    buffer_component_init(&self->buffer_component, &self->action_tx);
    status_component_init(&self->status_component, &self->action_tx);
    self->is_running = true;
}

void editor_fini(Editor *self)
{
    buffer_component_fini(&self->buffer_component);
    status_component_fini(&self->status_component);
    queue_fini(&self->event_tx);
    queue_fini(&self->action_tx);
}

void editor_exec(Editor *self)
{
    view_init();
    while (self->is_running)
    {
        component_draw(&self->buffer_component);
        component_draw(&self->status_component);
        WARN_CHECK_INT(tb_present(), == TB_OK);

        Event *event = calloc(1, sizeof(Event));
        CHECK(event);
        view_poll(event);
        queue_push(&self->event_tx, event);

        editor_handle_events(self);
        editor_handle_actions(self);
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void editor_handle_events(Editor *self)
{
    Event *event;
    while (queue_pop(&self->event_tx, (void **)(&event)))
    {
        switch (event->type)
        {
        case EVENT_TYPE_KEY:
            editor_handle_events_key(self, &event->data.key);
            break;
        case EVENT_TYPE_TICK:
            break;
        }

        component_handle_event(&self->buffer_component, event);
        component_handle_event(&self->status_component, event);
        free(event);
    }
}

void editor_handle_actions(Editor *self)
{
    Action *action;
    while (queue_pop(&self->action_tx, (void **)(&action)))
    {
        INFO("action_tx >>> %d", action->type);

        //
        if (action->type == ACTION_TYPE_QUIT)
        {
            self->is_running = false;
        }

        component_handle_action(&self->buffer_component, action);
        component_handle_action(&self->status_component, action);
        free(action);
    }
}

void editor_handle_events_key(Editor *self, KeyData *data)
{
    INFO("event_tx >>> Key(%c)", (char)(data->ch));
    bool is_ctrl = data->mod == TB_MOD_CTRL;
    bool is_e = data->ch == 'e';
    if (is_ctrl && is_e)
    {
        self->is_running = false;
    }
}
