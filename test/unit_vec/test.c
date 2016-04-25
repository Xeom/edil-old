#include "test.h"
#include "container/vec.h"

#define TEST_LEN 100000

void test_insert_end(void);
void test_insert_start(void);

static void test_print_vec(vec *v)
{
    size_t ind;

    for (ind = 0; ind < vec_len(v); ind++)
    {
        int *val;
        val = vec_item(v, ind);

        TEST(ind, lu);

        if (!val)
        {
            puts("v_item(v, ind) -> NULL");
            TEST(vec_err_str(), s);
        }

        else
            TEST(*(int *)vec_item(v, ind), d);
    }
}

void test_insert_end(void)
{
    vec *v;
    size_t ind;

    v = vec_init(sizeof(int));

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        int inv;
        inv = -(int)ind;

        TEST(ind, lu);
        TEST(inv, d);
        TEST(vec_len(v), lu);
        TEST(vec_insert(v, vec_len(v), 1, &inv), d);
    }

    test_print_vec(v);

    vec_free(v);
}

void test_insert_start(void)
{
    vec *v;
    size_t ind;

    v = vec_init(sizeof(int));

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        int inv;
        inv = -(int)ind;

        TEST(ind, lu);
        TEST(inv, d);
        TEST(vec_len(v), lu);
        TEST(vec_insert(v, 0, 1, &inv), d);
    }

    test_print_vec(v);

    vec_free(v);
}

int main(void)
{
    RUNTEST(insert_end);
    RUNTEST(insert_start);
}
