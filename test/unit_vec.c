#include <stdio.h>
#include <stdlib.h>
#include "vec.h"
#include "test.h"

#define TEST_VEC_RELATIONSHIP(vec, lambda)      \
{                                               \
    int i, *value;i = 0;                        \
    while (i < vec_len(vec))                    \
    {                                           \
        value = (int *)vec_item(vec, i);        \
        if (value == NULL)                      \
        {                                       \
            fputs(vec_err_str(), stderr);       \
            return -1;                          \
        }                                       \
        TEST_ASSERT(*value == (lambda),                             \
                    "insert_start: Item %d is %d. Should be %d",    \
                    i, *value, (lambda));                           \
        ++i;                                                        \
    }                                                               \
}


int test_insert_start(void)
{
    int *value, i;
    vec *v;

    v = vec_init(sizeof(int));
    i = 0;

    while (i++ < 500)
        vec_insert_start(v, 1, &i);

    TEST_VEC_RELATIONSHIP(v, 500 - i);

    vec_free(v);

    return 0;
}

int test_insert_end(void)
{
    int *value, i;
    vec *v;

    v = vec_init(sizeof(int));
    i = 0;

    while (i++ < 500)
        vec_insert_end(v, 1, &i);

    TEST_VEC_RELATIONSHIP(v, i + 1);

    vec_free(v);

    return 0;
}

int test_delete(void)
{
    int *value, i;
    vec *v;

    v = vec_init(sizeof(int));
    i = 0;

    while (i++ < 500)
        vec_insert_end(v, 1, &i);

    i = 0;
    while (i++ < 250)
        vec_delete(v, 0, 1);

    TEST_VEC_RELATIONSHIP(v, i + 250);

    vec_free(v);

    return 0;
}

int test_find(void)
{
    int *value, i;
    vec *v;

    v = vec_init(sizeof(int));
    i = 0;

    do
        vec_insert_end(v, 1, &i);
    while (i++ < 499);

    TEST_VEC_RELATIONSHIP(v, vec_find(v, &i));

    vec_free(v);

    return 0;
}

int test_rfind(void)
{
    int *value, i;
    vec *v;

    v = vec_init(sizeof(int));
    i = 0;

    do
        vec_insert_end(v, 1, &i);
    while (i++ < 499);

    TEST_VEC_RELATIONSHIP(v, vec_rfind(v, &i));

    vec_free(v);

    return 0;
}

int test_cut_start(void);

int test_cut_end(void);

int test_cut_middle(void);

main()
{
    TEST_DO(insert_start);
    TEST_DO(insert_end);
    TEST_DO(delete);
    TEST_DO(find);
    TEST_DO(rfind);
}
