#include <stdlib.h>
#include "textcont.h"

#include "line.h"
#include "vec.h"
#include "err.h"

textcont *textcont_init(void)
{
    textcont *rtn = malloc(sizeof(textcont));

    CHECK_ALLOC(textcont_init, rtn, NULL);

    rtn->currchunk = chunk_init();

    if (rtn->currchunk == NULL)
    {
        free(rtn);
        TRACE(textcont_init, chunk_init(), NULL);
    }

    return rtn;
}

line *textcont_insert(textcont *t, lineno n)
{
    size_t  offset;
    line   *rtn;
    chunk  *c;

    CHECK_NULL_PRM(textcont_insert, t, NULL);

    c = t->currchunk;
    c = chunk_find_containing(c, n);
    TRACE_NULL(textcont_insert, chunk_find_containing(c, n), c, NULL);
    t->currchunk = c;

    offset = n - chunk_get_startindex(c);

    rtn = line_init(c);

    if (chunk_insert_item(c, offset, rtn))
    {
        free(rtn);
        TRACE(textcont_insert, chunk_insert_item(c, offset, rtn), NULL);
    }

    TRACE_NULL(textcont_insert, line_init(c), rtn, 0);


    return rtn;
}

line *textcont_get_line(textcont *t, lineno n)
{
    size_t  offset;
    chunk  *c;

    CHECK_NULL_PRM(textcont_get_line, t, NULL);

    c = chunk_find_containing(t->currchunk, n);
    TRACE_NULL(textcont_get_line, chunk_find_containing(t->currchunk, n), c, NULL);
    t->currchunk = c;

    offset = n - chunk_get_startindex(c);

    return chunk_get_item(c, offset);
}

lineno textcont_get_total_lines(textcont *t)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_get_total_lines, t, INVALID_INDEX);

    c = t->currchunk;

    if (c == NULL)
        return INVALID_INDEX;

    while (! chunk_is_first(c))
        c = chunk_next(c);

    return chunk_len(c) + chunk_get_startindex(c);
}

int textcont_has_line(textcont *t, lineno ln)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_last_line, t, -1);

    c = t->currchunk;

    while ( (c = chunk_next(c)) )
        if (chunk_get_startindex(c) + chunk_len(c) > ln)
            return 1;

    return 0;
}

int textcont_is_last_line(textcont *t, line *l)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_last_line, t, -1);
    CHECK_NULL_PRM(textcont_is_last_line, l, -1);

    c = line_get_chunk(l);

    if (!chunk_is_last(c))
        return 0;

    if (chunk_get_item(c, chunk_len(c) - 1) == l)
        return 1;

    return 0;
}

int textcont_is_first_line(textcont *t, line *l)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_first_line, t, -1);
    CHECK_NULL_PRM(textcont_is_first_line, l, -1);

    c = line_get_chunk(l);

    if (!chunk_is_first(c))
        return 0;

    if (chunk_get_item(c, 0) != l)
        return 1;

    return 0;
}
