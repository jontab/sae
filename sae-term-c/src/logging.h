#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define DEBUG(FORMAT, ...) logging_printf(LOGGING_LEVEL_DEBUG, "[DEBUG] " FORMAT, ##__VA_ARGS__)
#define INFO(FORMAT, ...)  logging_printf(LOGGING_LEVEL_DEBUG, "[INFO]  " FORMAT, ##__VA_ARGS__)
#define WARN(FORMAT, ...)  logging_printf(LOGGING_LEVEL_DEBUG, "[WARN]  " FORMAT, ##__VA_ARGS__)
#define ERROR(FORMAT, ...) logging_printf(LOGGING_LEVEL_DEBUG, "[ERROR] " FORMAT, ##__VA_ARGS__)
#define FATAL(FORMAT, ...) logging_printf(LOGGING_LEVEL_DEBUG, "[FATAL] " FORMAT, ##__VA_ARGS__)

/******************************************************************************/
/* Enums                                                                      */
/******************************************************************************/

enum LoggingLevel
{
    LOGGING_LEVEL_DEBUG,
    LOGGING_LEVEL_INFO,
    LOGGING_LEVEL_WARN,
    LOGGING_LEVEL_ERROR,
    LOGGING_LEVEL_FATAL,
};

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef enum LoggingLevel LoggingLevel;

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void         logging_set_file(const char *file);
void         logging_set_level(LoggingLevel level);
void         logging_printf(LoggingLevel level, const char *format, ...);
LoggingLevel logging_level_parse(const char *arg);

#endif // LOGGING_H
