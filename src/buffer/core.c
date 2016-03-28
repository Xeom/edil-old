#define TYPED_VEC_TYPE cursor *
#define TYPED_VEC_NAME curs
#include "container/typed_vec.h"

struct buffer
{
    chunk    *currchunk;
    char     *name;
};

buffer *buffer_init(void);

void buffer_free(buffer *b);

static chunk *buffer_get_containing_chunk(buffer *b, lineno ln)
{
    c = b->currchunk;
    c = buffer_chunk_get_containing(c, ln);
    b->currchunk = c;
}

line *buffer_insert(buffer *b, lineno ln)
{
    line  *rtn;
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_get_lineno_offset(c, ln);

    rtn = buffer_chunk_insert_line(c, offset);

    return rtn;
}

int buffer_delete(buffer *b, lineno index);
{
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_get_lineno_offset(c, ln);

    buffer_chunk_delete(c, offset);

    return 0;
}

line *buffer_get_line(buffer *b, lineno ln);
{
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_get_lineno_offset(c, ln);

    rtn = buffer_chunk_get_line(c, offset);

    return rtn;
}

int buffer_set_line(buffer *b, lineno ln, line *l)
{
    chunk *c;
    lineno offset;

    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_get_lineno_offset(c, ln);

    buffer_chunk_set_line(c, offset, l);

    return 0;
}
