#include "munit.h"
#include "queue.h"

MunitResult unit_queue_push(const MunitParameter params[], void *data)
{
    Queue queue;
    queue_init(&queue);
    queue_push(&queue, (void *)(intptr_t)(1));
    munit_assert_ptr_not_null(queue.head);
    munit_assert_ptr_not_null(queue.tail);
    munit_assert_ptr_equal(queue.head, queue.tail);
    munit_assert_ptr_null(queue.head->next);
    munit_assert_ptr_null(queue.tail->next);
    queue_fini(&queue);
    return MUNIT_OK;
}

MunitResult unit_queue_pop(const MunitParameter params[], void *data)
{
    Queue queue;
    void *item;
    queue_init(&queue);
    munit_assert_false(queue_pop(&queue, &item));
    queue_push(&queue, (void *)(intptr_t)(1));
    munit_assert_true(queue_pop(&queue, &item));
    munit_assert_ptr_equal(item, (void *)(intptr_t)(1));
    munit_assert_ptr_null(queue.head);
    munit_assert_ptr_null(queue.tail);
    queue_fini(&queue);
    return MUNIT_OK;
}

MunitTest unit_queue_tests[] = {
    {"/push", unit_queue_push, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pop",  unit_queue_pop,  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL,    NULL,            NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};
