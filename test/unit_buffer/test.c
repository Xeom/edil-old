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
void test_del(void);
void test_for(void);
void test_find(void);

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
    }

    test_print_buffer(b);

    vec_free(v);
    buffer_free(b);
}

void test_insert_start(void)
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
        TEST(buffer_insert(b, 0), d);
        TEST(buffer_set_line(b, 0, v), d);
    }

    test_print_buffer(b);

    buffer_free(b);
    vec_free(v);
}

int main(void)
{
    RUNTEST(insert_end);
    RUNTEST(insert_start);
}
