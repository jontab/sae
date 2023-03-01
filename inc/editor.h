#ifndef EDITOR_H
#define EDITOR_H

#include "items.h"
#include "lines.h"
#include "list.h"
#include "network.h"
#include "options.h"

#include <curses.h>

typedef enum EditorMode
{
    NORMAL,
    INSERT,
    COMMAND,
} EditorMode;

typedef struct Editor
{
    Network    network;
    Options    options;
    EditorMode mode;
    Items      items;
    Lines      lines;
    int        cursor;
    int        clock;
    int        site;

    // Camera
    int        camx;
    int        camy;

    // Command
    char      *command;
    int        command_cursor;
    int        command_offset;
} Editor;

void editor_init              (Editor *e, int argc, char **argv);
void editor_exec              (Editor *e);
void editor_fini              (Editor *e);

int  editor_get_side_width    (Editor *e);

void editor_draw              (Editor *e);
void editor_draw_side         (Editor *e);
void editor_draw_main         (Editor *e);
void editor_draw_status       (Editor *e);
void editor_draw_cursor       (Editor *e);

void editor_retokenize        (Editor *e);

void editor_process           (Editor *e);
void editor_process_input     (Editor *e);
void editor_process_network   (Editor *e, int file);

void editor_run_normal        (Editor *e, int ch);
void editor_run_insert        (Editor *e, int ch);
void editor_run_command       (Editor *e, int ch);

int  editor_cursor_row        (Editor *e);
int  editor_cursor_col_visual (Editor *e);

void editor_scroll_up         (Editor *e);
void editor_scroll_down       (Editor *e);
void editor_scroll_left       (Editor *e);
void editor_scroll_right      (Editor *e);

void editor_set_char_next     (Editor *e);
void editor_set_char_prev     (Editor *e);

void editor_set_word_next     (Editor *e);
void editor_set_word_prev     (Editor *e);

void editor_set_line          (Editor *e, int line);
void editor_set_line_next     (Editor *e);
void editor_set_line_prev     (Editor *e);

void editor_delete_char       (Editor *e);
void editor_delete_word       (Editor *e);
void editor_delete_line       (Editor *e);

void editor_insert_char       (Editor *e, char ch);

void editor_insert_item       (Editor *e, Item *item);
void editor_delete_item       (Editor *e, int site, int clock);

void editor_load              (Editor *e);
void editor_save              (Editor *e, const char *filename);

#endif /* EDITOR_H */
