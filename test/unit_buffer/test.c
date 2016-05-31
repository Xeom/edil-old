#include "test.h"
#include "container/vec.h"
#include "buffer/buffer.h"

#if defined(TEST_REDUCED)
# define TEST_LEN 2000
#else
# define TEST_LEN 20000
#endif

void test_insert_end(void);
void test_insert_start(void);
void test_insert_middle(void);
void test_init(void);
void test_del(void);

static void test_print_buffer(buffer *b)
{
    size_t ind;

    PINFO("Printing items...");

    for (ind = 0; ind < buffer_len(b); ind++)
    {
        vec *l;

        TEST(ind, lu);

        l = buffer_get_line(b, ind);

        if (l == NULL)
            printf("buffer_get_line(b, ind) -> NULL");

        else
            TEST((char *)vec_item(l, 0), s);


        vec_free(l);
    }
}

void test_insert_end(void)
{
    vec *v;
    size_t ind;
    buffer *b;

    b = buffer_init();

    v = vec_init(10);
    vec_insert(v, 0, 10, "          ");

    PINFO("Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        sprintf(vec_item(v, 0), "%lu", ind);
        TEST(ind, lu);
        TEST(buffer_insert(b, ind), d);
        TEST(buffer_set_line(b, ind, v), d);
        TEST(buffer_len(b), lu);
    }

    test_print_buffer(b);

    vec_free(v);
    buffer_free(b);
}

void test_insert_start(void)
{
    size_t  ind;
    buffer *b;
    vec    *v;

    b = buffer_init();

    v = vec_init(10);
    vec_insert(v, 0, 10, "          ");

    PINFO("Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        sprintf(vec_item(v, 0), "%lu", ind);
        TEST(ind, lu);
        TEST(buffer_insert(b, 0), d);
        TEST(buffer_set_line(b, 0, v), d);
        TEST(buffer_len(b), lu);
    }

    test_print_buffer(b);

    buffer_free(b);
    vec_free(v);
}

void test_insert_middle(void)
{
    vec    *v;
    size_t  ind;
    buffer *b;

    b = buffer_init();

    v = vec_init(10);
    vec_insert(v, 0, 10, "          ");

    PINFO("Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        sprintf(vec_item(v, 0), "%lu", ind);
        TEST(ind, lu);
        TEST(buffer_insert(b,   ind / 2), d);
        TEST(buffer_set_line(b, ind / 2, v), d);
        TEST(buffer_len(b), lu);
    }

    test_print_buffer(b);

    buffer_free(b);
    vec_free(v);
}

void test_init(void)
{
    buffer *b;

    PINFO("Initializing with buffer_init...");

    b = buffer_init();
    TEST(b == NULL, d);

    buffer_free(b);
}

void test_del(void)
{
    vec   *v;
    size_t ind;
    buffer *b;

    b = buffer_init();

    v = vec_init(10);
    vec_insert(v, 0, 10, "          ");

    PINFO("Inserting items...");

    for (ind = 0; ind < TEST_LEN; ind++)
    {
        sprintf(vec_item(v, 0), "%lu", ind);
        buffer_insert(b, 0);
        buffer_set_line(b, 0, v);
    }

    PINFO("Doing sparse delete...");

    ind = 0;
    while (ind < buffer_len(b))
    {
        if (!(ind / 20 % 2 && ind % 7 && ind % 11))
        {
            TEST(ind, lu);
            TEST(buffer_delete(b, ind), d);
            TEST(buffer_len(b),        lu);
        }

        ++ind;
    }

    test_print_buffer(b);

    PINFO("Doing full delete...");

    while (buffer_len(b))
    {
        TEST(buffer_delete(b, 0), d);
        TEST(buffer_len(b),      lu);
    }

    test_print_buffer(b);

    buffer_free(b);
    vec_free(v);
}

int main(void)
{
    RUNTEST(insert_end);
    RUNTEST(insert_start);
    RUNTEST(insert_middle);
    RUNTEST(init);
    RUNTEST(del);
}
