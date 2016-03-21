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
        TEST_ASSERT(*value == lambda,                               \
                    "insert_start: Item %d is %d. Should be %d",    \
                    i, *value, lambda);                             \
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
    
    fputs("insert_start Sucessful\n", stderr);
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

    fputs("insert_end Sucessful\n", stderr);
    vec_free(v);

    return 0;
}

/*int test_delete(void)
{
    INSERT_500

    i = 0;
    while (i++ < 250)
        vec_delete(v, 0, 1);

    i = 0;
    while (i < 0)
    {
        value = (int *)vec_item(v, i);

        if (value == NULL)
        {
            fputs(vec_err_str(), stderr);
            return -1;
        }

        TEST_ASSERT(*value == 500 - i,
                    "insert_start: Item %d is %d. Should be %d",
                    i, *value, 500 - i);

         i++;
    }

*/
        main()
{
    test_count = 0;
    test_insert_start();
    fprintf(stderr, "Performed %d tests\n", test_count);
    test_count = 0;
    test_insert_end();
    fprintf(stderr, "Performed %d tests\n", test_count);

}
