#include <stdlib.h>
#include <string.h>

#include "container/table.h"
#include "container/hashes.h"
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
    table *properties;
    int    batch_enabled;
    lineno batch_start;
    lineno batch_end;
};

hook_add(buffer_on_batch_region, 3);

hook_add(buffer_line_on_delete_pre,  2);
hook_add(buffer_line_on_delete_post, 2);
hook_add(buffer_line_on_insert_pre,  2);
hook_add(buffer_line_on_insert_post, 2);

hook_add(buffer_line_on_change_pre,  3);
hook_add(buffer_line_on_change_post, 3);

hook_add(buffer_on_create, 1);
hook_add(buffer_on_delete, 1);

/* buffer_prop_set sets properties to these values */
const char *buffer_property_true  = "TRUE";
const char *buffer_property_false = "FALSE";

/* The properties managed by the buffer system, and set/get'd by *
 * buffer_prop[get/set]                                          */
const char *buffer_readonly_property = "__READONLY";
const char *buffer_modified_property = "__MODIFIED";
const char *buffer_locked_property   = "__LOCKED";

/*
 * Set a one of the properties defined by const chars above to either true
 * or false. (readonly, modified, or locked)
 *
 * @param b     A pointer to the buffer to set a property of.
 * @param name  The name of the property to set. The part of its
 *              buffer_*_property name that replaces the wildcard.
 * @param value Either true or false (literal, not strings or values) - The
 *              new value for this property.
 *
 * @return       0 on sucess, -1 on error.
 *
 */
#define buffer_prop_set(b, name, value)         \
    ctable_set(b->properties,                    \
               buffer_ ## name ## _property,    \
               buffer_property_ ## value);

/* Inline function wrapped by buffer_prop_get */
static inline int buffer_prop_get_(buffer *b, const char *property)
{
    char *val;

    val = ctable_get(b->properties, property);

    if (!val)
        return 0;

    return strcmp(val, buffer_property_true) == 0;
}

/*
 * Get the value of one of the properties defined by the const chars above, as a
 * bool. If the value is defined as FALSE or simply not defined, 0 is returned.
 *
 * @param b    A pointer to the buffer to get a property from.
 * @param name The name of the property to get. The part of its buffer_*property
 *             name that replaces the wildcard.
 *
 * @return     1 if the property is defined as "TRUE", 0 otherwise.
 *
 */
#define buffer_prop_get(b, name) \
    buffer_prop_get_(b, buffer_ ## name ## _property)

/*
 * Call a buffer hook with any number of arguments. This function sets the
 * buffer_locked_property property to buffer_property_true before calling the
 * hook, and buffer_property_false after it returns. This prevents hooks
 * calling more buffer functions recursively, or at least warns users if they
 * try and do it.
 *
 * @param hook A struct hook_s to call.
 * @param b    A pointer to the buffer to set lock flags on. The hook is called
 *             with a pointer to this buffer as its first argument.
 * @param ...  Any further arguments to call the hook with. Like all hook
 *             parameters, they should all be the size of a void*.
 *
 */
#define hook_call_buf(hook, b, ...)                \
    do {                                           \
        buffer_prop_set(b, locked, true);          \
        hook_call(hook, b, __VA_ARGS__);           \
        buffer_prop_set(b, locked, false);         \
    } while (0)

/*
 * Gets the chunk containing a specific line in a buffer. This function also
 * moves the currchunk of that buffer to the one containing the line, which is
 * why it is used in this file in place of chunk functions.
 *
 * @param b  The buffer to get a chunk from.
 * @param ln The linenumber to search for - a chunk will be returned that
 *           contains this linenumber. If the linenumber is greater than the
 *           number of lines in the buffer, the last chunk is returned.
 *
 * @return   A pointer to the relevant chunk.
 *
 */
static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln);

/* Initialize and return a new and empty buffer */
buffer *buffer_init(void)
{
    buffer *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(buffer)),
               terminal, return NULL);

    /* We need to initialize a new chunk for the buffer as well */
    TRACE_PTR(rtn->currchunk = buffer_chunk_init(),
              free(rtn);
              return NULL);

    TRACE_PTR(rtn->properties = table_init(sizeof(void *), sizeof(void *),
                                           hashes_key_str, hashes_eq_str, NULL),
              free(rtn->currchunk);
              free(rtn);
              return NULL);

    /* Default states of buffer properties */
    buffer_prop_set(rtn, readonly, false);
    buffer_prop_set(rtn, modified, false);
    buffer_prop_set(rtn, locked,   false);

    rtn->batch_enabled = 0;

    hook_call(buffer_on_create, rtn);

    return rtn;
}

void buffer_free(buffer *b)
{
    if (!b) return;

    ASSERT(buffer_prop_get(b, locked) == 0, high, return);

    hook_call(buffer_on_delete, b);

    /* Free the chunks (and lines) */
    buffer_chunk_free(b->currchunk);

    ctable_free(b->properties);

    /* Free the actual buffer */
    free(b);
}

int buffer_batch_start(buffer *b)
{
    b->batch_enabled = 1;
    b->batch_start   = INVALID_INDEX;
    b->batch_end     = INVALID_INDEX;

    return 0;
}

int buffer_batch_end(buffer *b)
{
    b->batch_enabled = 0;

    if (b->batch_start == INVALID_INDEX)
        return 0;

    hook_call(buffer_on_batch_region, b, &(b->batch_start), &(b->batch_end));

    return 0;
}

table *buffer_get_properties(buffer *b)
{
    return b->properties;
}

static inline chunk *buffer_get_containing_chunk(buffer *b, lineno ln)
{
    chunk *c;

    c            = buffer_chunk_get_containing(b->currchunk, ln);
    /* This variable acts like a cache to speed up the linked list */
    b->currchunk = c;

    return c;
}
/*
line *buffer_get_line_struct(buffer *b, lineno ln)
{
    lineno offset;
    chunk *c;

    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return NULL);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return NULL);

    return vec_lines_get((vec_lines *)c, offset);
}
*/
int buffer_insert(buffer *b, lineno ln)
{
    lineno offset;
    chunk *c;

    ASSERT_PTR(b, high,
               return -1);

    ASSERT(buffer_prop_get(b, locked) == 0, high, return -1);

    /* Get the chunk, and depth into that chunk of where we want to insert. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    if (!b->batch_enabled)
        hook_call_buf(buffer_line_on_insert_pre, b, &ln);

    if (buffer_prop_get(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot insert to read-only buffer.");

        return -1;
    }

    buffer_prop_set(b, modified, true);

    /* Aaaaand we've abstracted over the rest cos we're useless. */
    TRACE_INT(buffer_chunk_insert_line(c, offset),
              return -1);

    if (!b->batch_enabled)
        hook_call_buf(buffer_line_on_insert_post, b, &ln);

    else
    {
        b->batch_end  += 1;
        b->batch_end   = MAX(b->batch_end, ln);
        b->batch_start = MIN(b->batch_start, ln);
    }

    return 0;
}

int buffer_delete(buffer *b, lineno ln)
{
    lineno offset;
    chunk *c;

    ASSERT_PTR(b, high,
               return -1);

    ASSERT(buffer_prop_get(b, locked) == 0, high, return -1);

    /* Get the chunk, and depth into that chunk of where we want to delete. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    if (!b->batch_enabled)
        hook_call_buf(buffer_line_on_delete_pre, b, &ln);

    if (buffer_prop_get(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot delete from read-only buffer.");

        return -1;
    }

    buffer_prop_set(b, modified, true);

    /* Get a valid chunk */
    TRACE_PTR(c = buffer_chunk_delete_line(c, offset),
              return -1);

    /* Set the new current chunk to the valid chunk */
    b->currchunk = c;

    if (!b->batch_enabled)
        hook_call_buf(buffer_line_on_delete_post, b, &ln);

    else
    {
        b->batch_end  -= 1;
        b->batch_end   = MAX(b->batch_end, ln);
        b->batch_start = MIN(b->batch_start, ln);
    }

    return 0;
}

vec *buffer_get_line(buffer *b, lineno ln)
{
    lineno offset;
    vec  *rtn;
    chunk *c;

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
    lineno offset;
    chunk *c;

    ASSERT_PTR(b, high,
               return -1);

    if (!b->batch_enabled)
        ASSERT(buffer_prop_get(b, locked) == 0, high, return -1);

    /* Get the chunk, and depth into that chunk of where we want to get. */
    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return -1);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return -1);

    hook_call_buf(buffer_line_on_change_pre, b, &ln, v);

    if (buffer_prop_get(b, readonly))
    {
        ERR_NEW(medium, "Buffer is read-only",
                "Cannot set in read-only buffer.");

        return -1;
    }

    buffer_prop_set(b, modified, true);

    TRACE_INT(buffer_chunk_set_line(c, offset, v),
              return -1);

    if (!b->batch_enabled)
        hook_call_buf(buffer_line_on_change_post, b, &ln, v);

    else
    {
        b->batch_end   = MAX(b->batch_end, ln);
        b->batch_start = MIN(b->batch_start, ln);
    }

    return 0;
}

lineno buffer_len(buffer *b)
{
    ASSERT(b, high, return 0);

    /* Wrap the chunk function */
    return buffer_chunk_get_total_len(b->currchunk);
}

size_t buffer_len_line(buffer *b, lineno ln)
{
    lineno offset;
    chunk *c;
    line  *l;

    TRACE_PTR(c      = buffer_get_containing_chunk(b, ln),
              return 0);
    TRACE_IND(offset = buffer_chunk_lineno_to_offset(c, ln),
              return 0);

    ASSERT_PTR(l = vec_lines_get((vec_lines *)c, offset), high,
               return 0);

    return buffer_line_len(l);
}
