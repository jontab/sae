#include "munit.h"
#include "text/char.h"

MunitResult unit_char_index_compare(const MunitParameter params[], void *data)
{
    CharIndex x[] = {
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 0, 0, 0},
    };
    munit_assert_int(char_index_compare(&x[0], &x[1]), ==, -1);
    munit_assert_int(char_index_compare(&x[1], &x[2]), ==, -1);
    munit_assert_int(char_index_compare(&x[2], &x[3]), ==, -1);
    return MUNIT_OK;
}

MunitResult unit_char_index_search(const MunitParameter params[], void *data)
{
    CharIndex x[] = {
        {0, 0, 0, 1},
        {0, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 0, 0, 0},
    };
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &x[0]), ==, 0);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &x[1]), ==, 1);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &x[2]), ==, 2);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &x[3]), ==, 3);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &(CharIndex){0, 0, 0, 0}), ==, 0);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &(CharIndex){0, 0, 0, 2}), ==, 1);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &(CharIndex){0, 0, 2, 0}), ==, 2);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &(CharIndex){0, 2, 0, 0}), ==, 3);
    munit_assert_int(char_index_search(x, sizeof(x) / sizeof(CharIndex), &(CharIndex){2, 0, 0, 0}), ==, 4);
    return MUNIT_OK;
}

MunitTest unit_char_tests[] = {
    {"/index_compare", unit_char_index_compare, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/index_search",  unit_char_index_search,  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL,             NULL,                    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};
