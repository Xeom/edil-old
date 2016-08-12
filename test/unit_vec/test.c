#include "test.h"
#include "container/vec.h"

#if defined(TEST_REDUCED)
# define TEST_LEN 1000
# define TEST_FIND_LEN 800
#else
# define TEST_LEN 20000
# define TEST_FIND_LEN 2000
#endif

void test_insert_end(void);
void test_insert_start(void);
void test_del(void);
void test_for(void);
void test_find(void);

static void test_print_vec(vec *v)
{
    size_t ind;

    PLOG(info, "Printing items...");

    for (ind = 0; ind < vec_len(v); ind++)
    {
        int *val;
        val = vec_item(v, ind);

        TEST(ind, lu);

        if (!val)
            puts("v_item(v, ind) -> NULL");


        else
            TEST(*(int *)vec_item(v, ind), d);
    }
}

void test_insert_end(void)
{
    vec *v;
    size_t ind;

    v = vec_init(sizeof(int));

    PLOG(info, "Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        int inv;
        inv = -(int)ind;

        TEST(ind, lu);
        TEST(inv, d);
        TEST(vec_len(v), lu);
        TEST(vec_insert(v, ind, 1, &inv), d);
    }

    test_print_vec(v);

    vec_free(v);
}

void test_insert_start(void)
{
    vec *v;
    size_t ind;

    v = vec_init(sizeof(int));

    PLOG(info, "Inserting items...");

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

void test_for(void)
{
    vec *v;
    size_t ind;
    int    item;

    v = vec_init(sizeof(int));

    PLOG(info, "Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        item = (int)ind;
        vec_insert(v, ind, 1, &item);
    }

    PLOG(info, "Doing vec_foreach...");

    vec_foreach(v, int, item,
                TEST(_vec_index, lu);
                TEST(item,        d);
        );

    PLOG(info, "Doing vec_rforeach...");

    vec_rforeach(v, int, item,
                 TEST(_vec_index, lu);
                 TEST(item,        d);
        );

    vec_free(v);
}

void test_del(void)
{
    vec *v;
    size_t ind;

    v = vec_init(sizeof(int));

    PLOG(info, "Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        int item;

        item = (int)ind;
        vec_insert(v, ind, 1, &item);
    }

    PLOG(info, "Doing sparse delete...");

    ind = 0;
    while (ind < vec_len(v))
    {
        if (!(ind / 20 % 2 && ind % 7 && ind % 11))
        {
            TEST(ind, lu);
            TEST(vec_delete(v, ind, 1), d);
            TEST(vec_len(v),           lu);
        }

        ++ind;
    }

    test_print_vec(v);

    PLOG(info, "Doing full delete...");

    while (vec_len(v))
    {
        TEST(vec_delete(v, 0, 1),  d);
        TEST(vec_len(v),          lu);
    }

    test_print_vec(v);

    vec_free(v);
}

void test_find(void)
{
    vec *v;
    int item;
    size_t ind;

    v = vec_init(sizeof(int));

    PLOG(info, "Inserting items...");

    for (ind = 0; ind < TEST_FIND_LEN; ind++)
    {
        item = (int)ind;
        vec_insert(v, ind, 1, &item);
    }

    PLOG(info, "Searching vector...");

    for (item = 0; item < TEST_FIND_LEN; item++)
    {
        static const int map = 0x7fffffff;
        TEST(item, d);
        TEST(vec_find(v, &item),     lu);
        TEST(vec_rfind(v, &item),    lu);
        TEST(vec_contains(v, &item),  d);
        TEST(vec_bisearch(v, &item, &map), lu);
    }

    vec_free(v);

    v = vec_init(sizeof(int));

    for (ind = 0; ind < TEST_FIND_LEN; ind++)
    {
        item = (int)ind & (~0x1);
        vec_insert(v, ind, 1, &item);
    }

    for (item = 0; item < TEST_FIND_LEN + 1; item++)
    {
        static const int map = 0x7fffffff;
        TEST(vec_bisearch(v, &item, &map), lu);
    }

    PLOG(info, "Trying vec_contains with out of bounds values...");

    item = -1;
    TEST(item, d);
    TEST(vec_contains(v, &item), d);

    item = TEST_FIND_LEN + 1;
    TEST(item, d);
    TEST(vec_contains(v, &item), d);
    vec_free(v);
}

int main(void)
{
    RUNTEST(insert_end);
    RUNTEST(insert_start);
    RUNTEST(for);
    RUNTEST(del);
    RUNTEST(find);
}
