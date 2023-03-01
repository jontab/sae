#include "core.h"
#include "editor.h"

#include <stdio.h>
#include <stdlib.h>

static Editor E;

void init(int argc, char **argv)
{
    editor_init(&E, argc, argv);
    DIE_IF(initscr()                == NULL, "initscr failed");
    DIE_IF(noecho()                 == ERR,  "noecho failed");
    DIE_IF(cbreak()                 == ERR,  "cbreak failed");
    DIE_IF(keypad(stdscr, TRUE)     == ERR,  "keypad failed");
    DIE_IF(nodelay(stdscr, TRUE)    == ERR,  "nodelay failed");
    WARNING_IF(start_color()        == ERR,  "start_color failed");
    WARNING_IF(use_default_colors() == ERR,  "use_default_colors failed");
}

void fini(void)
{
    editor_fini(&E);
    endwin();
    fprintf(stderr, "status: %s\n", get_status());
}

int main(int argc, char **argv)
{
    init(argc, argv);
    atexit(fini);
    editor_exec(&E);
    return 0;
}
