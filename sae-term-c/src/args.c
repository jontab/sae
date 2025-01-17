#include "args.h"
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void args_init(Args *self);
static void args_usage(FILE *out);

static struct option long_options[] = {
    {"help",      no_argument,       NULL, 'h'},
    {"log-level", required_argument, NULL, 'L'},
    {"log-path",  required_argument, NULL, 'l'},
    {NULL,        0,                 NULL, 0  },
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void args_parse(Args *self, int argc, char **argv)
{
    args_init(self);
    while (1)
    {
        int opt = getopt_long(argc, argv, "hl:L:", long_options, NULL);
        if (opt < 0)
        {
            break;
        }

        switch (opt)
        {
        case 'h':
            args_usage(stdout);
            exit(EXIT_SUCCESS);
        case 'l':
            self->logging_path = optarg;
            break;
        case 'L':
            self->logging_level = logging_level_parse(optarg);
            break;
        default:
            args_usage(stderr);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc)
    {
        self->filename = optarg;
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void args_init(Args *self)
{
    memset(self, 0, sizeof(Args));
    self->logging_level = LOGGING_LEVEL_INFO;
}

void args_usage(FILE *out)
{
    fprintf(out, "Usage: sae [<filename>] [<options>]\n");
    fprintf(out, "\n");
    fprintf(out, "Positional arguments:\n");
    fprintf(out, "  filename          File to edit\n");
    fprintf(out, "\n");
    fprintf(out, "Optional arguments:\n");
    fprintf(out, "  -h, --help        Show this help message and exit\n");
    fprintf(out, "  -l, --log-path    Log path\n");
    fprintf(out, "  -L, --log-level   Log level (choices: debug, info, warn, error, fatal; default: info)\n");
}
