#include <stdlib.h>

#include "test.h"
#include "container/table.h"

#define TEST_LEN 46341

void test_init(void);
void test_set(void);
void test_del(void);

static void test_print_table(table *tbl)
{
    int ind;

    PINFO("Printing items...");

    for (ind = 1; ind < TEST_LEN; ind++)
    {
        int *val;

        val = table_get(tbl, &ind);

        TEST(ind, d);

        if (!val)
            puts("table_get(tbl, &ind) -> NULL");
        else
            TEST(*(int *)table_get(tbl, &ind), d);
    }
}

void test_set(void)
{
    int    ind;
    table *tbl;

    tbl = table_init(sizeof(int), sizeof(unsigned int),
                     NULL, NULL, NULL);

    PINFO("Inserting items...");

    for (ind = 1; ind < TEST_LEN; ind++)
    {
        int sqr;
        sqr = ind * ind;
        TEST(ind, d);
        TEST(sqr, d);
        TEST(table_set(tbl, &ind, &sqr), d);
    }

    test_print_table(tbl);

    table_free(tbl);
}

void test_del(void)
{
    int    ind;
    table *tbl;

    tbl = table_init(sizeof(int), sizeof(int),
                     NULL, NULL, NULL);

    PINFO("Inserting items...");

    for (ind = 1; ind < TEST_LEN; ind++)
    {
        TEST(ind, d);
        TEST(table_set(tbl, &ind, &ind), d);
        TEST(table_len(tbl), lu);
    }

    PINFO("Deleting items sparsely...");

    for (ind = 1; ind < TEST_LEN; ind++)
    {
        if ((ind / 20) % 2 && ind % 7 && ind % 11)
            continue;

        TEST(ind, d);
        TEST(table_delete(tbl, &ind), d);
        TEST(table_len(tbl), lu);
    }

    test_print_table(tbl);

    PINFO("Deleting all items...");

    for (ind = 1; ind < TEST_LEN; ind++)
    {
        if (!((ind / 20) % 2 && ind % 7 && ind % 11))
            continue;

        TEST(ind, d);
        TEST(table_delete(tbl, &ind), d);
        TEST(table_len(tbl), lu);
    }

    test_print_table(tbl);

    table_free(tbl);
}

void test_init(void)
{
    table *tbl;

    PINFO("Initializing with table_init...");

    tbl = table_init(sizeof(char), sizeof(char), NULL, NULL, NULL);
    TEST(tbl == NULL, d);

    table_free(tbl);
    tbl = malloc(sizeof(table));
    TEST(tbl == NULL, d);

    PINFO("Initializing with table_create...");

    tbl = table_create(tbl, sizeof(char), sizeof(char), NULL, NULL, NULL);
    TEST(tbl == NULL, d);

    table_free(tbl);
}

int main(void)
{
    RUNTEST(init);
    RUNTEST(set);
    RUNTEST(del);

    return 0;
}
