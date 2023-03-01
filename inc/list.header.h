#ifndef LIST_HEADER_H
#define LIST_HEADER_H

#define LIST_GENERATE_HEADER(T, N, P)           \
                                                \
    typedef struct N                            \
    {                                           \
        T  *data;                               \
        int size;                               \
        int capacity;                           \
    } N;                                        \
                                                \
    N   *P##_create  (int capacity);            \
    void P##_reserve (N *list, int capacity);   \
    void P##_insert  (N *list, T data, int at); \
    void P##_append  (N *list, T data);         \
    void P##_prepend (N *list, T data);         \
    T    P##_take    (N *list, int at);         \
    void P##_remove  (N *list, int at);         \
    void P##_clear   (N *list);                 \
    void P##_reset   (N *list);                 \
    void P##_delete  (N *list);                 \

#endif /* LIST_HEADER_H */
