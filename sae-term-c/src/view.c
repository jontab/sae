#define TB_IMPL

#include "view.h"
#include "check.h"
#include "events/event.h"
#include "logging.h"
#include "termbox2.h"

static void view_fini(void);
static void view_poll_key(struct tb_event *ev, Event *out);
static void view_poll_mouse(struct tb_event *ev, Event *out);
static void view_poll_resize(struct tb_event *ev, Event *out);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void view_init(void)
{
    CHECK_INT(tb_init(), == TB_OK);
    CHECK_INT(atexit(view_fini), == 0);
}

void view_poll(Event *out)
{
    struct tb_event ev;
    int             ret = tb_poll_event(&ev);
    WARN_CHECK_INT(ret, != TB_ERR);
    if (ret == TB_OK)
    {
        switch (ev.type)
        {
        case TB_EVENT_KEY:
            view_poll_key(&ev, out);
            break;
        case TB_EVENT_MOUSE:
            view_poll_mouse(&ev, out);
            break;
        case TB_EVENT_RESIZE:
            view_poll_resize(&ev, out);
            break;
        }
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void view_fini(void)
{
    WARN_CHECK_INT(tb_shutdown(), == TB_OK);
}

void view_poll_key(struct tb_event *ev, Event *out)
{
    out->type = EVENT_TYPE_KEY;
    out->data.key = (KeyData){
        .ch = ev->ch,
        .mod = ev->mod,
        .key = ev->key,
    };
}

void view_poll_mouse(struct tb_event *ev, Event *out)
{
    (void)(ev);
    memset(out, 0, sizeof(Event));
}

void view_poll_resize(struct tb_event *ev, Event *out)
{
    (void)(ev);
    memset(out, 0, sizeof(Event));
}
