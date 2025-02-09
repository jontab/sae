#ifndef ARGS_H
#define ARGS_H

#include "logging.h"

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct Args Args;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct Args
{
    const char  *filename;
    const char  *logging_path;
    LoggingLevel logging_level;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void args_parse(Args *self, int argc, char **argv);

#endif // ARGS_H
