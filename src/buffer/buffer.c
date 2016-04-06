#include <stdlib.h>

#include "buffer/chunk.h"
#include "buffer/line.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#include "buffer/buffer.h"

struct buffer_s
{
    chunk    *currchunk;
    char     *name;
};

hook_add(buffer_line_on_delete, 2);
hook_add(buffer_line_on_insert, 2);

hook_add(buffer_on_create, 1);
hook_add(buffer_on_delete, 1);

/* Function to get the chunk containinng a specific line in a buffer.   *
 * This function also moves the currchunk of that buffer to the one     *
 * containing the line, which is why I use it instead of simply calling *
 * the chunk API.                                                       */
static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln);

/* Initialize and return a new and empty buffer */
buffer *buffer_init(void)
{
    buffer *rtn;

    rtn = malloc(sizeof(buffer));

    /* We need to initialize a new chunk for the buffer as well */
    rtn->currchunk = buffer_chunk_init(rtn);
    rtn->name = NULL;

    hook_call(buffer_on_create, &rtn);

    return rtn;
}

void buffer_free(buffer *b)
{
    hook_call(buffer_on_delete, &b);

    /* Free the chunks (and lines) */
    buffer_chunk_free(b->currchunk);

    /* Free the buffer's name */
    if (b->name)
        free(b->name);

    /* Free the actual buffer */
    free(b);
}

static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln)
{
    /* Setting the currchunk to the chunk we last visited makes for  *
     * much faster accesses, assuming accesses are fairly localized. */
    b->currchunk = buffer_chunk_get_containing(b->currchunk, ln);

    return b->currchunk;
}

int buffer_insert(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;

    /* Get the chunk, and depth into that chunk of where we want to insert. */
    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    /* Aaaaand we've abstracted over the rest cos we're useless. */
    return buffer_chunk_insert_line(c, offset);
}

int buffer_delete(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;

    /* Get the chunk, and depth into that chunk of where we want to delete. */
    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    /* Bla */
    buffer_chunk_delete_line(c, offset);

    return 0;
}

vec *buffer_get_line(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;
    vec  *rtn;

    /* Get the chunk, and depth into that chunk of where we want to get. */
    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    rtn    = buffer_chunk_get_line(c, offset);

    return rtn;
}

int buffer_set_line(buffer *b, lineno ln, vec *v)
{
    chunk *c;
    lineno offset;

    /* Get the chunk, and depth into that chunk of where we want to get. */
    c      = buffer_get_containing_chunk(b, ln);
    offset = buffer_chunk_lineno_to_offset(c, ln);

    return buffer_chunk_set_line(c, offset, v);
}

lineno buffer_len(buffer *b)
{
    /* Just wrap the chunk function */
    return buffer_chunk_get_total_len(b->currchunk);
}
