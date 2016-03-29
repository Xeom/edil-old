#include <stdlib.h>

#include "container/vec.h"
#include "buffer/chunk.h"

#include "buffer/line.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE char
#define VEC_TYPED_NAME char
#include "container/typed_vec.h"


struct line_s
{
    vec text;
    chunk   *c;
};

line *buffer_line_init(void)
{
    line *rtn = malloc(sizeof(line));

    vec_char_create(rtn);

    return rtn;
}

void buffer_line_free(line *l)
{
    vec_char_free((vec_lines *)l);
}

int buffer_line_set_chunk(line *l, chunk *c)
{
    l->c = c;

    return 0;
}

lineno buffer_line_get_lineno(line *l)
{
    lineno offset;
    chunk *c;

    c = l->c;

    offset = vec_lines_find((vec_lines *)c, &l);

    return buffer_chunk_offset_to_lineno(c, offset);
}

lineno buffer_line_rget_lineno(line *l)
{
    lineno offset;
    chunk *c;

    c = l->c;

    offset = vec_lines_rfind((vec_lines *)c, &l);

    return buffer_chunk_offset_to_lineno(c, offset);
}

lineno buffer_line_get_lineno_hint(line *l, lineno hint)
{
    lineno hintoffset;
    lineno deviation, maxdeviation;
    chunk *c;

    c = l->c;

    if (buffer_chunk_offset_to_lineno(c, 0) > hint)
        return buffer_line_rget_lineno(l);

    hintoffset = buffer_chunk_lineno_to_offset(c, hint);

    if (vec_lines_len((vec_lines *)c) <= hintoffset)
        return buffer_line_get_lineno(l);

    maxdeviation = MAX(hintoffset, vec_lines_len((vec_lines *)c) - hintoffset - 1);
    deviation = 0;
    while (++deviation <= maxdeviation)
    {
        lineno offset;

        offset = hintoffset - deviation;
        if (deviation <= hintoffset
            && vec_lines_get((vec_lines *)c, offset) == l)
            return offset;

        offset = hintoffset + deviation;
        if (offset < vec_lines_len((vec_lines *)c) &&
            vec_lines_get((vec_lines *)c, offset) == l)
            return offset;
    }

    return INVALID_INDEX;
}
