#include "editor.h"

#include <assert.h>
#include <ctype.h>
#include <curses.h>
#include <stdarg.h>
#include <sys/select.h>
#include <time.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define TABSIZ 4

void editor_init(Editor *e, int argc, char **argv)
{
    options_parse(&e->options, argc, argv);

    // Network
    DIE_IF(!network_start(&e->network,
                          e->options.host,
                          e->options.port,
                          e->options.mode),
           "network_start");

    if (e->options.mode == CLIENT)
    {
        network_request_file(&e->network);
    }
    else if (e->options.filename)
    {
        editor_load(e);
    }

    editor_retokenize(e);

    // Site
    srand(time(NULL));
    e->site = rand();
}

void editor_exec(Editor *e)
{
    while (1)
    {
        editor_draw(e);
        editor_process(e);
    }
}

void editor_fini(Editor *e)
{
    network_stop(&e->network);
    items_reset(&e->items);
    lines_reset(&e->lines);

    if (e->options.filename)
        DELETE(e->options.filename, free);

    if (e->command)
        DELETE(e->command, free);
}

int editor_get_side_width(Editor *e)
{
    int max = e->camy + LINES - 1;
    int cnt = 1;

    while (max)
    {
        cnt++;
        max /= 10;
    }

    return cnt;
}

void editor_draw(Editor *e)
{
    editor_draw_main(e);
    editor_draw_side(e);
    editor_draw_status(e);
    editor_draw_cursor(e);
    refresh();
}

void editor_draw_side(Editor *e)
{
    char buffer[16];
    int width = editor_get_side_width(e);

    attron(A_BOLD);
    for (int y = 0, i = e->camy; y < LINES - 1; y++, i++)
    {
        if (i < e->lines.size)
        {
            snprintf(buffer, sizeof(buffer), "%d", i);
        }
        else
        {
            // We don't want to show line numbers for lines that aren't in the file.
            buffer[0] = '\0';
        }

        move(y, 0);
        for (int x = 0; x < width; x++)
            addch((x < strlen(buffer)) ? buffer[x] : ' ');
    }

    attroff(A_BOLD);
}

static void editor_draw_main_helper(Editor *e, int offx, int liney)
{
    for (int i = e->lines.data[liney].begin, viscol = 0; (i < e->lines.data[liney].end) && (viscol < e->camx + COLS - offx); i++)
    {
        if (e->items.data[i]->deleted)
            continue;

        if (e->items.data[i]->value == '\t')
        {
            int spaces = TABSIZ - (viscol % TABSIZ);
            for (int k = 0; (k < spaces) && (viscol < COLS - offx); k++, viscol++)
                if (viscol >= e->camx)
                    addch(' ');
        }
        else
        {
            if (viscol++ >= e->camx)
                addch(e->items.data[i]->value);
        }
    }
}

void editor_draw_main(Editor *e)
{
    int offx = editor_get_side_width(e);
    for (int y = 0, i = e->camy; y < LINES - 1; y++, i++)
    {
        move(y, offx);
        if (y < e->lines.size)
            editor_draw_main_helper(e, offx, i);
        clrtoeol();
    }
}

void editor_draw_status(Editor *e)
{
    if (e->mode == COMMAND)
    {
        move(LINES - 1, 0);
        for (int x = 0, i = e->command_offset; (x < COLS) && (i < strlen(e->command)); x++, i++)
            addch(e->command[i]);
        clrtoeol();
    }
    else
    {
        move(LINES - 1, 0);

        // Mode
        attron(A_BOLD);
        addstr((e->mode == NORMAL) ? "Normal" : "Insert");
        attroff(A_BOLD);

        addstr(" | ");

        // Network
        attron(A_BOLD);
        addstr((e->network.file > 0) ? "Online" : "Offline");
        attroff(A_BOLD);

        addstr(" | ");

        // Filename
        addstr((e->options.filename)
                   ? e->options.filename
                   : "Untitled");

        // Status
        if (get_status())
        {
            addstr(" | ");
            addstr(get_status());
        }

        clrtoeol();
    }
}

void editor_draw_cursor(Editor *e)
{
    if (e->mode == COMMAND)
    {
        move(LINES - 1, e->command_cursor - e->command_offset);
    }
    else
    {
        move(editor_cursor_row(e) - e->camy,
             editor_get_side_width(e) + editor_cursor_col_visual(e) - e->camx);
    }
}

void editor_retokenize(Editor *e)
{
    lines_clear(&e->lines);

    Line line;
    line.begin = 0;

    for (int i = 0; i < e->items.size; i++)
    {
        if (!e->items.data[i]->deleted && (e->items.data[i]->value == '\n'))
        {
            line.end = i;
            lines_append(&e->lines, line);
            line.begin = i + 1;
        }
    }

    line.end = e->items.size;
    lines_append(&e->lines, line);
}

void editor_process(Editor *e)
{
    fd_set set;
    FD_ZERO(&set);

    int max = 0;
    FD_SET(0, &set);

    network_add_files_set(&e->network, &set, &max);

    assert(select(max + 1, &set, NULL, NULL, NULL) >= 0);

    if (FD_ISSET(0, &set))
        editor_process_input(e);

    for (int i = 1; i < max + 1; i++)
        if (FD_ISSET(i, &set))
            network_handle(&e->network, i, e);
}

void editor_process_input(Editor *e)
{
    switch (e->mode)
    {
    case NORMAL:
        editor_run_normal(e, getch());
        break;

    case INSERT:
        editor_run_insert(e, getch());
        break;

    default:
        editor_run_command(e, getch());
        break;
    }
}

void editor_run_normal(Editor *e, int ch)
{
    // TODO: More keybinds for normal mode
    switch (ch)
    {
    case ':':
        e->mode = COMMAND;
        e->command = estrdup(":");
        e->command_cursor = 1;
        e->command_offset = 0;
        break;

    case 'i':
        e->mode = INSERT;
        break;

    case KEY_LEFT:
    case 'h':
        editor_set_char_prev(e);
        break;

    case KEY_DOWN:
    case 'j':
        editor_set_line_next(e);
        break;

    case KEY_UP:
    case 'k':
        editor_set_line_prev(e);
        break;

    case KEY_RIGHT:
    case 'l':
        editor_set_char_next(e);
        break;
    }
}

void editor_run_insert(Editor *e, int ch)
{
    switch (ch)
    {
    case 27:
        e->mode = NORMAL;
        break;

    case KEY_HOME:
    {
        int row = editor_cursor_row(e);
        e->cursor = e->lines.data[row].begin;
        editor_scroll_left(e);
    }
    break;

    case KEY_END:
    {
        int row = editor_cursor_row(e);
        e->cursor = e->lines.data[row].end;
        editor_scroll_right(e);
    }
    break;

    case KEY_DOWN:
        editor_set_line_next(e);
        break;

    case KEY_LEFT:
        editor_set_char_prev(e);
        break;

    case KEY_RIGHT:
        editor_set_char_next(e);
        break;

    case KEY_UP:
        editor_set_line_prev(e);
        break;

    case KEY_BACKSPACE:
        editor_set_char_prev(e);
        if (e->cursor < e->items.size)
        {
            network_delete(&e->network,
                           e->items.data[e->cursor]->site,
                           e->items.data[e->cursor]->clock);
            editor_delete_char(e);
        }
        break;

    default:
        if (isprint(ch) || (ch == '\n') || (ch == '\t'))
        {
            editor_insert_char(e, ch);
            editor_set_char_next(e);
        }

        break;
    }
}

void editor_run_command(Editor *e, int ch)
{
    switch (ch)
    {
    case '\n':
        // TODO: Add more commands
        if (strcmp(e->command, ":q") == 0)
        {
            exit(0);
        }
        else if ((strlen(e->command) >= 2) &&
                 (e->command[0] == ':') &&
                 (e->command[1] == 'w'))
        {
            // We need to locate the first word after a space.
            const char *filename = strchr(e->command, ' ');
            while (filename && *filename && isspace(*filename))
                filename++;
            editor_save(e, (!filename || (*filename == '\0')) ? NULL : filename);
        }

        e->mode = NORMAL;
        e->command_cursor = 0;
        e->command_offset = 0;
        DELETE(e->command, free);
        break;

    case KEY_BACKSPACE:
        if (e->command_cursor)
        {
            editor_run_command(e, KEY_LEFT);
            editor_run_command(e, KEY_DC);
        }
        break;

    case KEY_DC:
        if (e->command_cursor < strlen(e->command))
            memmove(e->command + e->command_cursor,
                    e->command + e->command_cursor + 1,
                    strlen(e->command) - e->command_cursor + 1);
        break;

    case KEY_END:
        e->command_cursor = strlen(e->command);
        while (e->command_cursor >= e->command_offset + COLS)
            e->command_offset++;
        break;

    case KEY_HOME:
        e->command_cursor = 0;
        e->command_offset = 0;
        break;

    case KEY_LEFT:
        if (e->command_cursor)
            e->command_cursor--;
        while (e->command_cursor < e->command_offset)
            e->command_offset--;
        break;

    case KEY_RIGHT:
        if (e->command_cursor < strlen(e->command))
            e->command_cursor++;
        while (e->command_cursor >= e->command_offset + COLS)
            e->command_offset++;
        break;

    default:
        if (isprint(ch))
        {
            e->command = erealloc(e->command, strlen(e->command) + 2);
            memmove(e->command + e->command_cursor + 1,
                    e->command + e->command_cursor,
                    strlen(e->command) - e->command_cursor + 1);
            e->command[e->command_cursor] = ch;
            editor_run_command(e, KEY_RIGHT);
        }
        break;
    }
}

int editor_cursor_row(Editor *e)
{
    assert(e->lines.size > 0);
    for (int y = 0; y < e->lines.size; y++)
        if ((e->lines.data[y].begin <= e->cursor) &&
            (e->cursor <= e->lines.data[y].end))
            return y;
    return e->lines.size - 1;
}

int editor_cursor_col_visual(Editor *e)
{
    assert(e->lines.size > 0);
    int row = editor_cursor_row(e);
    int col_visual = 0;
    for (int i = e->lines.data[row].begin; (i < e->lines.data[row].end) && (i < e->cursor); i++)
    {
        if (e->items.data[i]->deleted)
            continue;
        col_visual += (e->items.data[i]->value == '\t')
                          ? TABSIZ - (col_visual % TABSIZ)
                          : 1;
    }

    return col_visual;
}

void editor_scroll_up(Editor *e)
{
    int row = editor_cursor_row(e);
    if (row < e->camy)
        e->camy = row;
}

void editor_scroll_down(Editor *e)
{
    int row = editor_cursor_row(e);
    int bot = e->camy + LINES - 2;
    if (bot < row)
        e->camy += (row - bot);
}

void editor_scroll_left(Editor *e)
{
    int col = editor_cursor_col_visual(e);
    if (col < e->camx)
        e->camx = col;
}

void editor_scroll_right(Editor *e)
{
    int col = editor_cursor_col_visual(e);
    int rig = e->camx + (COLS - editor_get_side_width(e) - 1);
    if (rig < col)
        e->camx += (col - rig);
}

void editor_set_char_next(Editor *e)
{
    if (e->cursor >= e->items.size)
        return;

    // If we are hovering over a deleted character, we should skip over the
    // first existing item we see, because our cursor is visually over it.
    bool skip = e->items.data[e->cursor]->deleted;
    int i = e->items.size;

    for (int next = e->cursor + 1; next < e->items.size; next++)
    {
        if (!e->items.data[next]->deleted)
        {
            if (skip)
            {
                skip = false;
                continue;
            }

            i = next;
            break;
        }
    }

    e->cursor = i;

    // Camera
    editor_scroll_left(e);
    editor_scroll_right(e);
    editor_scroll_down(e);
}

void editor_set_char_prev(Editor *e)
{
    // Our cursor, if it is over a deleted element, is hovering over the
    // next existing element. So, we don't need to skip.
    for (int prev = e->cursor - 1; prev >= 0; prev--)
    {
        if (!e->items.data[prev]->deleted)
        {
            e->cursor = prev;
            break;
        }
    }

    // Camera
    editor_scroll_left(e);
    editor_scroll_right(e);
    editor_scroll_up(e);
}

void editor_set_word_next(Editor *e)
{
    // TODO
}

void editor_set_word_prev(Editor *e)
{
    // TODO
}

void editor_set_line(Editor *e, int line)
{
    // TODO
}

void editor_set_line_next(Editor *e)
{
    int row = editor_cursor_row(e);
    if (row < e->lines.size - 1)
    {
        int col = line_offset_calculate(&e->lines.data[row], &e->items, e->cursor);
        Line next = e->lines.data[row + 1];
        e->cursor = line_offset_add(&next, &e->items, col);
    }

    editor_scroll_down(e);
    editor_scroll_left(e);
}

void editor_set_line_prev(Editor *e)
{
    int row = editor_cursor_row(e);
    if (row > 0)
    {
        int col = line_offset_calculate(&e->lines.data[row], &e->items, e->cursor);
        Line prev = e->lines.data[row - 1];
        e->cursor = line_offset_add(&prev, &e->items, col);
    }

    editor_scroll_up(e);
    editor_scroll_left(e);
}

void editor_delete_char(Editor *e)
{
    // Our cursor, if it is over a deleted element, is hovering over the
    // next existing element. So, we need to search to the right to find it.
    for (int i = e->cursor; i <= e->items.size; i++)
    {
        if (!e->items.data[e->cursor]->deleted)
        {
            editor_delete_item(e,
                               e->items.data[e->cursor]->site,
                               e->items.data[e->cursor]->clock);
            break;
        }
    }
}

void editor_delete_word(Editor *e)
{
    // TODO
}

void editor_delete_line(Editor *e)
{
    // TODO
}

void editor_insert_char(Editor *e, char ch)
{
    Item *item = ecalloc(1, sizeof(Item));
    item->site = e->site;
    item->clock = e->clock++;

    item->parent_site = -1;
    item->parent_clock = -1;

    if (e->cursor)
    {
        item->parent_site = e->items.data[e->cursor - 1]->site;
        item->parent_clock = e->items.data[e->cursor - 1]->clock;
    }

    item->value = ch;
    item->deleted = false;

    network_insert(&e->network, item);
    editor_insert_item(e, item);
}

void editor_insert_item(Editor *e, Item *item)
{
    e->clock = (e->clock < item->clock) ? item->clock : e->clock;
    int at = items_insort(&e->items, item);
    if (at < e->cursor)
        e->cursor++;
    editor_retokenize(e);
}

void editor_delete_item(Editor *e, int site, int clock)
{
    int i = items_find(&e->items, site, clock);
    if (i >= 0)
    {
        e->items.data[i]->deleted = true;
        editor_retokenize(e);
    }
}

void editor_load(Editor *e)
{
    assert(e->options.filename);
    assert(e->items.size == 0);

    FILE *file = fopen(e->options.filename, "r");
    if (file == NULL)
    {
        set_status("Failed to load from file");
        return;
    }

    int par = -1;
    for (char c = fgetc(file); c != EOF; c = fgetc(file), par++)
    {
        Item *item = ecalloc(1, sizeof(Item));
        item->site = e->site;
        item->clock = e->clock++;

        if (par >= 0)
        {
            // TODO: Do we need to go back to last undeleted item?
            item->parent_site = e->items.data[par]->site;
            item->parent_clock = e->items.data[par]->clock;
        }
        else
        {
            item->parent_site = -1;
            item->parent_clock = -1;
        }

        item->value = c;
        item->deleted = false;

        editor_insert_item(e, item);
    }

    fclose(file);
    editor_retokenize(e);
}

void editor_save(Editor *e, const char *filename)
{
    set_status("Banana");
    if (filename == NULL)
        filename = e->options.filename;

    if (filename == NULL)
    {
        set_status("Filename not provided");
        return;
    }

    FILE *out = fopen(filename, "w");
    if (out == NULL)
    {
        set_status("Failed to open file for writing");
        return;
    }

    int bytes = 0;
    for (int i = 0; i < e->items.size; i++)
    {
        if (!e->items.data[i]->deleted)
        {
            if (fputc(e->items.data[i]->value, out) < 0)
            {
                set_status("Failed to write to file");
                break;
            }
            else
            {
                bytes++;
            }
        }
    }

    fclose(out);
    set_status("Wrote %d bytes to file", bytes);

    if (e->options.filename == NULL)
        e->options.filename = estrdup(filename);
}
