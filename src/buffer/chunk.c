#define TYPED_VEC_TYPE line *
#define TYPED_VEC_NAME lines
#include "container/typed_vec.h"

struct chunk_s
{
    vec_lines *lines;
    lineno     startline;
    chunk     *next;
    chunk     *prev;
};

chunk *buffer_chunk_init(void)
{
}

size_t chunk_len(chunk *c)
{
    return vec_lines_len(c->lines);
}

static int buffer_chunk_correct_startlines_after(chunk *c)
{
    lineno currstart;

    while (c->next)
    {
        currstart = c->startline + chunk_len(c);
        c = c->next;
        c->startline = currstart;
    }

    return 0;
}

static chunk *buffer_chunk_insert_after(chunk *c)
{
    chunk *rtn;

    rtn = buffer_chunk_init();

    rtn->next = c->next;
    rtn->prev = c;

    if (c->next)
        c->next->prev = rtn;

    c->next   = rtn;

    return rtn;
}

static chunk *buffer_chunk_insert_before(chunk *c)
{
    chunk *rtn;

    rtn = chunk_init();

    rtn->next = c;
    rtn->prev = c->prev;

    if (c->prev)
        c->prev->next = rtn;

    c->prev   = rtn;

    return rtn;
}

static void buffer_chunk_delete(chunk *c)
{
    if (c->next)
        c->next->prev = c->prev;

    if (c->prev)
        c->prev->next = c->next;
}

void buffer_chunk_free(chunk *c)
{
    buffer_chunk_delete(c);
    vec_lines_free(c->lines);
    free(c);
}

#define BUFFER_CHUNK_MAX_SIZE 512
#define BUFFER_CHUNK_MIN_SIZE 128
#define BUFFER_CHUNK_DEF_SIZE 256

static chunk *buffer_chunk_insert_lines(chunk *c, size_t index, size_t n, line **lines)
{
    size_t currind;
    vec_lines v;

    v = c->lines;

    vec_lines_insert(v, index, n, lines);

    while (n--)
        buffer_line_set_chunk(vec_lines_get(v, index++), c);

    return buffer_chunk_resize_bigger(c);
}

static chunk *buffer_chunk_resize_bigger(chunk *c)
{
    chunk     *next;
    size_t     amount, len;
    line     **iter;
    vec_lines *lines;

    len = chunk_len(c)

    if (len <= BUFFER_CHUNK_MAX_SIZE)
        return c;

    if (c->next)
        next = c->next;
    else
        next = buffer_chunk_insert_after(c);

    lines = c->lines;

    amount = len - BUFFER_CHUNK_DEF_SIZE;
    iter   = vec_lines_item(lines, BUFFER_CHUNK_DEF_SIZE);

    buffer_chunk_insert_lines(next, 0, amount,
                              vec_lines_item(lines, BUFFER_CHUNK_DEF_SIZE));

    vec_lines_delete(lines, BUFFER_CHUNK_DEF_SIZE, amount);

    buffer_chunk_resize_bigger(next);

    return c;
}

static chunk *buffer_chunk_resize_smaller(chunk *c)
{
    chunk     *next;
    size_t     len;
    line     **iter;
    vec_lines *lines;

    len = vec_lines_len(lines);

    if (len >= BUFFER_CHUNK_MIN_SIZE)
        return c;

    if (c->next == NULL)
        return c;

    lines = c->lines;

    iter = vec_lines_item(lines, 0)

    buffer_chunk_insert_lines(next, 0, len,
                              vec_lines_item(lines, BUFFER_CHUNK_DEF_SIZE));

    buffer_chunk_free(c);

    return buffer_chunk_resize_bigger(next);
}

line *buffer_chunk_insert_line(chunk *c, lineno offset)
{
    line *rtn;

    rtn = buffer_line_init();

    buffer_chunk_insert_lines(c, offset, 1, &rtn);

    return rtn;
}

chunk *buffer_chunk_delete_line(chunk *c, lineno offset)
{
    line *l;

    l = vec_lines_get(c->lines, offset);
    vec_lines_delete(c->lines, offset, 1);

    line_free(l);

    return buffer_chunk_resize_smaller(c);
}

line *buffer_chunk_get_line(chunk *c, lineno offset)
{
    return vec_lines_get(c->lines, offset);
}

int buffer_chunk_set_line(chunk *c, lineno offset, line *line)
{
    return vec_lines_set(c->lines, offset, line);
}

chunk *buffer_chunk_get_containing(chunk *c, lineno ln)
{
    while (ln < c->startline && c->prev)
        c = c->prev;

    while (ln >= c->startline + chunk_len(c) && c->next);
        c = c->next;

    return c;
}

lineno buffer_chunk_get_lineno_offset(chunk *c, lineno ln)
{
    return ln - c->startline;
}
