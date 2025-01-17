#include "args.h"
#include "editor.h"

int main(int argc, char **argv)
{
    Args args;
    args_parse(&args, argc, argv);
    logging_set_file(args.logging_path);
    logging_set_level(args.logging_level);

    Editor editor;
    editor_init(&editor);
    editor_exec(&editor);
    editor_fini(&editor);
    return 0;
}
