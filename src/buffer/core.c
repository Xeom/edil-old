/*#define VEC_TYPED_TYPE cursor *
#define VEC_TYPED_NAME curs
#include "container/typed_vec.h"
*/
#include <stdlib.h>

#include "buffer/chunk.h"
#include "buffer/line.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#include "buffer/core.h"

struct buffer_s
{
    chunk    *currchunk;
    char     *name;
};

static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln);

buffer *buffer_init(void)
{
    buffer *rtn;

    rtn = malloc(sizeof(buffer));
    vec_lines_create((vec_lines *)rtn);

    rtn->name = NULL;

    return rtn;
}

void buffer_free(buffer *b)
{
    if (b->name)
        free(b->name);

    vec_lines_free((vec_lines *)b);
}

static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln)
{
    b->currchunk = buffer_chunk_get_containing(b->currchunk, ln);

    return b->currchunk;
}

line *buffer_insert(buffer *b, lineno ln)
{
    line  *rtn;
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    rtn = buffer_chunk_insert_line(c, offset);

    return rtn;
}

int buffer_delete(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    buffer_chunk_delete_line(c, offset);

    return 0;
}

line *buffer_get_line(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;
    line  *rtn;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    rtn = buffer_chunk_get_line(c, offset);

    return rtn;
}
