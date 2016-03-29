#include <stdlib.h>

#include "buffer/line.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#include "buffer/chunk.h"

struct chunk_s
{
    vec_lines  lines;
    lineno     startline;
    chunk     *next;
    chunk     *prev;
};

static int buffer_chunk_correct_startlines(chunk *c);
static chunk *buffer_chunk_insert(chunk *c);
static void buffer_chunk_delete(chunk *c);
static chunk *buffer_chunk_resize_bigger(chunk *c);
static chunk *buffer_chunk_resize_smaller(chunk *c);
static void buffer_chunk_free_norecurse(chunk *c);

chunk *buffer_chunk_init(void)
{
    chunk *rtn;

    rtn = malloc(sizeof(chunk));

    vec_lines_create((vec_lines *)rtn);

    rtn->next = NULL;
    rtn->prev = NULL;

    rtn->startline = 0;

    return rtn;
}

static int buffer_chunk_correct_startlines(chunk *c)
{
    lineno currstart;

    if (c->prev)
        currstart = c->prev->startline + vec_lines_len((vec_lines *)c->prev);
    else
        currstart = 0;

    do
    {
        c->startline = currstart;
        currstart += vec_lines_len((vec_lines *)c);
        c = c->next;
    }
    while (c);

    return 0;
}

static chunk *buffer_chunk_insert(chunk *c)
{
    chunk *rtn;

    rtn = buffer_chunk_init();

    rtn->next = c->next;
    rtn->prev = c;

    if (c->next)
        c->next->prev = rtn;

    c->next   = rtn;

    rtn->startline = c->startline + vec_lines_len((vec_lines *)c);

    return rtn;
}

static void buffer_chunk_delete(chunk *c)
{
    if (c->next)
        c->next->prev = c->prev;

    if (c->prev)
        c->prev->next = c->next;

    buffer_chunk_free_norecurse(c);
}

static void buffer_chunk_free_norecurse(chunk *c)
{
    vec_foreach((vec *)c, line *, l,
                buffer_line_free(l));

    vec_lines_free((vec_lines *)c);
}

void buffer_chunk_free(chunk *c)
{
    chunk *curr, *next;

    curr = c->next;
    while (curr)
    {
        next = curr->next;
        buffer_chunk_free_norecurse(curr);
        curr = next;
    }

    curr = c->prev;
    while (curr)
    {
        next = curr->prev;
        buffer_chunk_free_norecurse(curr);
        curr = next;
    }

    buffer_chunk_free_norecurse(c);
}

#define BUFFER_CHUNK_MAX_SIZE 10
#define BUFFER_CHUNK_MIN_SIZE 2
#define BUFFER_CHUNK_DEF_SIZE 5

static chunk *buffer_chunk_insert_lines(chunk *c, size_t index, size_t n, line **lines)
{
    vec_lines_insert((vec_lines *)c, index, n, lines);

    while (n--)
        buffer_line_set_chunk(vec_lines_get((vec_lines *)c, index++), c);

    return buffer_chunk_resize_bigger(c);
}

static chunk *buffer_chunk_resize_bigger(chunk *c)
{
    chunk     *next;
    size_t     amount, len;
    line     **iter;

    len = vec_lines_len((vec_lines *)c);

    if (len <= BUFFER_CHUNK_MAX_SIZE)
        return c;

    if (c->next)
        next = c->next;
    else
        next = buffer_chunk_insert(c);

    amount = len - BUFFER_CHUNK_DEF_SIZE;
    iter   = vec_lines_item((vec_lines *)c, BUFFER_CHUNK_DEF_SIZE);

    buffer_chunk_insert_lines(next, 0, amount, iter);

    vec_lines_delete((vec_lines *)c, BUFFER_CHUNK_DEF_SIZE, amount);

    buffer_chunk_resize_bigger(next);

    return c;
}

static chunk *buffer_chunk_resize_smaller(chunk *c)
{
    chunk     *next;
    size_t     len;
    line     **iter;

    len = vec_lines_len((vec_lines *)c);

    next = c->next;

    if (len >= BUFFER_CHUNK_MIN_SIZE)
        return c;

    if (next == NULL)
        return c;

    iter = vec_lines_item((vec_lines *)c, 0);

    buffer_chunk_insert_lines(next, 0, len, iter);

    buffer_chunk_delete(c);

    return buffer_chunk_resize_bigger(next);
}

line *buffer_chunk_insert_line(chunk *c, lineno offset)
{
    line *rtn;

    rtn = buffer_line_init();

    buffer_chunk_insert_lines(c, offset, 1, &rtn);
    buffer_chunk_resize_bigger(c);
    buffer_chunk_correct_startlines(c);

    return rtn;
}

chunk *buffer_chunk_delete_line(chunk *c, lineno offset)
{
    line *l;

    l = vec_lines_get((vec_lines *)c, offset);
    vec_lines_delete((vec_lines *)c, offset, 1);

    buffer_line_free(l);

    c = buffer_chunk_resize_smaller(c);
    buffer_chunk_correct_startlines(c);

    return c;
}

line *buffer_chunk_get_line(chunk *c, lineno offset)
{
    return vec_lines_get((vec_lines *)c, offset);
}

chunk *buffer_chunk_get_containing(chunk *c, lineno ln)
{
    while (ln < c->startline && c->prev)
        c = c->prev;

    while (ln >= c->startline + vec_lines_len((vec_lines *)c) && c->next)
        c = c->next;

    return c;
}

lineno buffer_chunk_lineno_to_offset(chunk *c, lineno ln)
{

    return ln - c->startline;
}

lineno buffer_chunk_offset_to_lineno(chunk *c, lineno offset)
{
    return offset + c->startline;
}

lineno buffer_chunk_get_total_len(chunk *c)
{
    while (c->next)
        c = c->next;

    return c->startline + vec_lines_len((vec_lines *)c);
}
