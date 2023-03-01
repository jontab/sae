#ifndef LIST_SOURCE_H
#define LIST_SOURCE_H

#include "core.h"

#include <string.h>

#define LIST_DEFAULT_FREE(X) \
    do                       \
    {                        \
    } while (0)

#define LIST_GENERATE_SOURCE(T, N, P, F)                                                \
                                                                                        \
    N *P##_create(int capacity)                                                         \
    {                                                                                   \
        N *list = ecalloc(1, sizeof(N));                                                \
        P##_reserve(list, capacity);                                                    \
        return list;                                                                    \
    }                                                                                   \
                                                                                        \
    void P##_reserve(N *list, int capacity)                                             \
    {                                                                                   \
        if (list->capacity < capacity)                                                  \
        {                                                                               \
            list->capacity = list->size * 2 + 11;                                       \
            list->data = erealloc(list->data, list->capacity * sizeof(T));              \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    void P##_insert(N *list, T data, int at)                                            \
    {                                                                                   \
        P##_reserve(list, list->size + 1);                                              \
        memmove(list->data + at + 1, list->data + at, (list->size++ - at) * sizeof(T)); \
        list->data[at] = data;                                                          \
    }                                                                                   \
                                                                                        \
    void P##_append(N *list, T data)                                                    \
    {                                                                                   \
        P##_insert(list, data, list->size);                                             \
    }                                                                                   \
                                                                                        \
    void P##_prepend(N *list, T data)                                                   \
    {                                                                                   \
        P##_insert(list, data, 0);                                                      \
    }                                                                                   \
                                                                                        \
    T P##_take(N *list, int at)                                                         \
    {                                                                                   \
        T data = list->data[at];                                                        \
        memmove(list->data + at, list->data + at + 1, (--list->size - at) * sizeof(T)); \
        return data;                                                                    \
    }                                                                                   \
                                                                                        \
    void P##_remove(N *list, int at)                                                    \
    {                                                                                   \
        T data = P##_take(list, at);                                                    \
        F(data);                                                                        \
    }                                                                                   \
                                                                                        \
    void P##_clear(N *list)                                                             \
    {                                                                                   \
        for (int i = 0; i < list->size; i++)                                            \
            F(list->data[i]);                                                           \
        list->size = 0;                                                                 \
    }                                                                                   \
                                                                                        \
    void P##_reset(N *list)                                                             \
    {                                                                                   \
        P##_clear(list);                                                                \
        if (list->data)                                                                 \
            DELETE(list->data, free);                                                   \
        list->capacity = 0;                                                             \
    }                                                                                   \
                                                                                        \
    void P##_delete(N *list)                                                            \
    {                                                                                   \
        P##_reset(list);                                                                \
        free(list);                                                                     \
    }

#endif /* LIST_SOURCE_H */
