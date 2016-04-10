#include <stdlib.h>
#include <string.h>

#include "buffer/chunk.h"
#include "buffer/line.h"
#include "err.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#include "buffer/buffer.h"

struct buffer_s
{
    chunk *currchunk; /* Linked list (most recent link) of chunks */
    uint   flags;
};

hook_add(buffer_line_on_delete, 2);
hook_add(buffer_line_on_insert, 2);
hook_add(buffer_line_on_change, 2);

hook_add(buffer_on_create, 1);
hook_add(buffer_on_delete, 1);

#define buffer_extern_flags buffer_readonly_flag || buffer_modified_flag
#define buffer_flag(b, name)     (b->flags &   buffer_ ## name ## _flag)
#define buffer_flag_on(b, name)  (b->flags |=  buffer_ ## name ## _flag)
#define buffer_flag_off(b, name) (b->flags &= ~buffer_ ## name ## _flag)

/* Function to get the chunk containinng a specific line in a buffer.   *
 * This function also moves the currchunk of that buffer to the one     *
 * containing the line, which is why I use it instead of simply calling *
 * the chunk API.                                                       */
static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln);

/* Initialize and return a new and empty buffer */
buffer *buffer_init(void)
{
    buffer *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(buffer)),
               terminal, return NULL);

    /* We need to initialize a new chunk for the buffer as well */
    TRACE_PTR(rtn->currchunk = buffer_chunk_init(),
              return NULL);

    rtn->flags = 0;

    hook_call(buffer_on_create, &rtn);

    return rtn;
}

void buffer_free(buffer *b)
{
    if (!b) return;

    hook_call(buffer_on_delete, &b);

    /* Free the chunks (and lines) */
    buffer_chunk_free(b->currchunk);

    /* Free the actual buffer */
    free(b);
}

static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln)
{
    chunk *c;

    c            = buffer_chunk_get_containing(b->currchunk, ln);
    /* This variable acts like a cache to speed up the linked list */
    b->currchunk = c;

    return c;
}

int buffer_insert(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;

    ASSERT_PTR(b, high,
               return -1);

    if (buffer_flag(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot insert to read-only buffer.");

        return -1;
    }

    /* Get the chunk, and depth into that chunk of where we want to insert. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    buffer_flag_on(b, modified);

    /* Aaaaand we've abstracted over the rest cos we're useless. */
    TRACE_INT(buffer_chunk_insert_line(c, offset),
              return -1);

    hook_call(buffer_line_on_insert, &b, &ln);

    return 0;
}

int buffer_delete(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;

    ASSERT_PTR(b, high,
               return -1);

    /* Get the chunk, and depth into that chunk of where we want to delete. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    hook_call(buffer_line_on_delete, &b, &ln);

    if (buffer_flag(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot delete from read-only buffer.");

        return -1;
    }

    buffer_flag_on(b, modified);

    /* Bla */
    TRACE_INT(buffer_chunk_delete_line(c, offset),
              return -1);

    return 0;
}

vec *buffer_get_line(buffer *b, lineno ln)
{
    chunk *c;
    lineno offset;
    vec  *rtn;

    ASSERT_PTR(b, high,
               return NULL);

    /* Get the chunk, and depth into that chunk of where we want to get. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return NULL);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return NULL);

    TRACE_PTR(rtn    = buffer_chunk_get_line(c, offset),
              return NULL);

    return rtn;
}

int buffer_set_line(buffer *b, lineno ln, vec *v)
{
    chunk *c;
    lineno offset;

    ASSERT_PTR(b, high,
               return -1);

    if (buffer_flag(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot set in read-only buffer.");

        return -1;
    }

    /* Get the chunk, and depth into that chunk of where we want to get. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    buffer_flag_on(b, modified);

    TRACE_INT(buffer_chunk_set_line(c, offset, v),
              return -1);

    hook_call(buffer_line_on_change, &b, &ln);

    return 0;
}

int buffer_get_flag(buffer *b, uint flag)
{
    return (b->flags & flag) != 0;
}

int buffer_enable_flag(buffer *b, uint flag)
{
    if (flag & ~buffer_extern_flags)
    {
        ERR_NEW(high, "Invalid flag",
                "Flag either does not exist or cannot be externally set");
        return -1;
    }

    b->flags |= flag;

    return 0;
}

int buffer_disable_flag(buffer *b, uint flag)
{
    if (flag & ~buffer_extern_flags)
    {
        ERR_NEW(high, "Invalid flag",
                "Flag either does not exist or cannot be externally set");
        return -1;
    }

    b->flags &= ~flag;

    return 0;
}

lineno buffer_len(buffer *b)
{
    /* Just wrap the chunk function */
    return buffer_chunk_get_total_len(b->currchunk);
}
