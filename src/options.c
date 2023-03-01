#include "options.h"
#include "core.h"

#include <getopt.h>
#include <stdlib.h>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "8080"

void options_parse(Options *o, int argc, char **argv)
{
    o->host = DEFAULT_HOST;
    o->port = DEFAULT_PORT;
    o->mode = OFF;

    static struct option longopts[] = {
        {"host", required_argument, 0, 'i'},
        {"port", required_argument, 0, 'p'},
        {"client", no_argument, 0, 'c'},
        {"server", no_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
    };

    int option, index;
    while ((option = getopt_long(argc, argv, "i:p:csh", longopts, &index)) != -1)
    {
        switch (option)
        {
        case 'i':
            o->host = optarg;
            break;

        case 'p':
            o->port = optarg;
            break;

        case 'c':
            o->mode = CLIENT;
            break;

        case 's':
            o->mode = SERVER;
            break;

        case 'h':
            options_usage(stdout, *argv);
            exit(0);

        default:
            exit(1);
        }
    }

    if (optind < argc)
        o->filename = estrdup(argv[optind]);
}

void options_usage(FILE *out, const char *program)
{
    fprintf(out, "Usage: %s [FILENAME] [OPTIONS]\n", program);
    fprintf(out, "\n");
    fprintf(out, "A simple & collaborative editor for the terminal!\n");
    fprintf(out, "\n");
    fprintf(out, "Options:\n");
    fprintf(out, "    -i HOST, --host HOST    Host of server (default: " DEFAULT_HOST ")\n");
    fprintf(out, "    -p PORT, --port PORT    Port of server (default: " DEFAULT_PORT ")\n");
    fprintf(out, "    -c, --client            Run as client (default: false)\n");
    fprintf(out, "    -s, --server            Run as server (default: false)\n");
    fprintf(out, "    -h, --help              Show this help message and exit\n");
}
