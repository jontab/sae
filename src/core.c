#include "core.h"

#include <stdarg.h>
#include <string.h>

static char status[BUFSIZ];

void *ecalloc(size_t count, size_t size)
{
    void *data = calloc(count, size);
    DIE_IF(size && !data, "calloc failed");
    return data;
}

void *erealloc(void *data, size_t size)
{
    data = realloc(data, size);
    DIE_IF(size && !data, "realloc failed");
    return data;
}

char *estrdup(const char *data)
{
    char *copy = strdup(data);
    DIE_IF(!copy, "strdup failed");
    return copy;
}

void set_status(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(status, sizeof(status), format, args);
    va_end(args);
}

const char *get_status(void)
{
    return status;
}
