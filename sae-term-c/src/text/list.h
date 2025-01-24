#ifndef LIST_H
#define LIST_H

#include "../check.h"
#include <stddef.h>
#include <string.h>

#define LIST_GENERATE_HEADER(NAME, PREFIX, TYPE)                                                                       \
    typedef struct NAME NAME;                                                                                          \
                                                                                                                       \
    struct NAME                                                                                                        \
    {                                                                                                                  \
        TYPE  *data;                                                                                                   \
        size_t size;                                                                                                   \
        size_t capacity;                                                                                               \
    };                                                                                                                 \
                                                                                                                       \
    void PREFIX##_init(NAME *self);                                                                                    \
    void PREFIX##_fini(NAME *self);                                                                                    \
    void PREFIX##_clear(NAME *self);                                                                                   \
    void PREFIX##_reserve(NAME *self, size_t size);                                                                    \
    void PREFIX##_insert(NAME *self, TYPE data, size_t at);                                                            \
    void PREFIX##_append(NAME *self, TYPE data);                                                                       \
    TYPE PREFIX##_pop(NAME *self, size_t at);                                                                          \
    void PREFIX##_remove(NAME *self, size_t at);

#define LIST_GENERATE_SOURCE(NAME, PREFIX, TYPE)                                                                       \
    void PREFIX##_init(NAME *self)                                                                                     \
    {                                                                                                                  \
        memset(self, 0, sizeof(NAME));                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_fini(NAME *self)                                                                                     \
    {                                                                                                                  \
        PREFIX##_clear(self);                                                                                          \
        if (self->data)                                                                                                \
        {                                                                                                              \
            free(self->data);                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        memset(self, 0, sizeof(NAME));                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_clear(NAME *self)                                                                                    \
    {                                                                                                                  \
        self->size = 0;                                                                                                \
        /* Note: Free. */                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_reserve(NAME *self, size_t size)                                                                     \
    {                                                                                                                  \
        if (self->capacity < size)                                                                                     \
        {                                                                                                              \
            self->capacity = size + 11;                                                                                \
            self->data = realloc(self->data, self->capacity * sizeof(TYPE));                                           \
            CHECK(self->data);                                                                                         \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_insert(NAME *self, TYPE data, size_t at)                                                             \
    {                                                                                                                  \
        PREFIX##_reserve(self, self->size + 1);                                                                        \
        memmove(&self->data[at + 1], &self->data[at], (self->size++ - at) * sizeof(TYPE));                             \
        self->data[at] = data;                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_append(NAME *self, TYPE data)                                                                        \
    {                                                                                                                  \
        PREFIX##_insert(self, data, self->size);                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    TYPE PREFIX##_pop(NAME *self, size_t at)                                                                           \
    {                                                                                                                  \
        TYPE data = self->data[at];                                                                                    \
        memmove(&self->data[at], &self->data[at + 1], (--self->size - at) * sizeof(TYPE));                             \
        return data;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    void PREFIX##_remove(NAME *self, size_t at)                                                                        \
    {                                                                                                                  \
        PREFIX##_pop(self, at);                                                                                        \
        /* Note: Free. */                                                                                              \
    }

#endif // LIST_H
