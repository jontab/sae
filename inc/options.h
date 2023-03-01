#ifndef OPTIONS_H
#define OPTIONS_H

#include "network.h"

#include <stdio.h>

typedef struct Options
{
    const char *host;
    const char *port;
    char       *filename;
    NetworkMode mode;
} Options;

void options_parse (Options *o, int argc, char **argv);
void options_usage (FILE *out, const char *program);

#endif /* OPTIONS_H */
