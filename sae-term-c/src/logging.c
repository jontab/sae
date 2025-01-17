#include "logging.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE        *logging_file = NULL;
static LoggingLevel logging_level = LOGGING_LEVEL_DEBUG;

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void logging_set_file(const char *file)
{
    logging_file = fopen(file, "w");
}

void logging_set_level(LoggingLevel level)
{
    logging_level = level;
}

void logging_printf(LoggingLevel level, const char *format, ...)
{
    if (!logging_file || (level < logging_level))
    {
        return;
    }

    time_t     now = time(NULL);
    struct tm *tm = localtime(&now);
    fprintf(logging_file, "[%02d:%02d:%02d] ", tm->tm_hour, tm->tm_min, tm->tm_sec);

    va_list args;
    va_start(args, format);
    vfprintf(logging_file, format, args);
    va_end(args);
}

LoggingLevel logging_level_parse(const char *arg)
{
    if (strcmp(arg, "debug") == 0)
    {
        return LOGGING_LEVEL_DEBUG;
    }

    if (strcmp(arg, "info") == 0)
    {
        return LOGGING_LEVEL_INFO;
    }

    if (strcmp(arg, "warn") == 0)
    {
        return LOGGING_LEVEL_WARN;
    }

    if (strcmp(arg, "error") == 0)
    {
        return LOGGING_LEVEL_ERROR;
    }

    if (strcmp(arg, "fatal") == 0)
    {
        return LOGGING_LEVEL_FATAL;
    }

    return atoi(arg);
}
