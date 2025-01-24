#ifndef CHECK_H
#define CHECK_H

#include "logging.h"
#include <stdlib.h>

#define CHECK_INT(X, PRED)                                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        int ret = (X);                                                                                                 \
        if (!(ret PRED))                                                                                               \
        {                                                                                                              \
            FATAL("check failed: " #X #PRED ", value: %d", ret);                                                       \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)

#define CHECK(X)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(X))                                                                                                      \
        {                                                                                                              \
            FATAL("check failed: " #X);                                                                                \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)

#define WARN_CHECK_INT(X, PRED)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        int _ret = (X);                                                                                                \
        if (!(_ret PRED))                                                                                              \
        {                                                                                                              \
            WARN("check failed: " #X #PRED ", value: %d", _ret);                                                       \
        }                                                                                                              \
    } while (0)

#endif // CHECK_H
