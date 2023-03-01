#ifndef CORE_H
#define CORE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DELETE(X, F) \
    do               \
    {                \
        F(X);        \
        X = NULL;    \
    } while (0)

#define DIE_IF(COND, MSG, ...)                        \
    do                                                \
    {                                                 \
        if (COND)                                     \
        {                                             \
            set_status(MSG, ##__VA_ARGS__);           \
            fprintf(stderr, MSG "\n", ##__VA_ARGS__); \
            exit(1);                                  \
        }                                             \
    } while (0)

#define WARNING_IF(COND, MSG, ...)      \
    do                                  \
    {                                   \
        set_status(MSG, ##__VA_ARGS__); \
    } while (0)

void       *ecalloc    (size_t count, size_t size);
void       *erealloc   (void *data, size_t size);
char       *estrdup    (const char *data);
void        set_status (const char *format, ...);
const char *get_status (void);

#endif /* CORE_H */
