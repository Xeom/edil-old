#include <stdlib.h>

#include "buffer/buffer.h"
#include "buffer/line.h"
#include "err.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#include "buffer/chunk.h"

#define BUFFER_CHUNK_MAX_SIZE 512 /* The maximum size a buffer can be before it overflows            */
#define BUFFER_CHUNK_MIN_SIZE 128 /* The minimum size a buffer can be if it is not the last buffer   */
#define BUFFER_CHUNK_DEF_SIZE 256 /* The default size a buffer returns to after over or underflowing */

struct chunk_s
{
    vec_lines  lines;     /* chunk_s is an extention of vec_s. The vector stores pointers to every line */
    lineno     startline; /* The line number of the first line of the chunk                             */
    chunk     *next;      /* A link forward to the next chunk. NULL if this is the last chunk.          */
    chunk     *prev;      /* A link backward to the last chunk. NULL if this is the first chunk.        */
};

/* Correct the starting lines of all the chunks after and including c */
static void buffer_chunk_correct_startlines(chunk *c);

/* Insert a new chunk after a particular chunk and return it. */
static chunk *buffer_chunk_insert(chunk *c);
/* Delete a chunk and sew up the surrounding chunks in the chain */
static void   buffer_chunk_delete(chunk *c);

/* Call when chunk sizes increase or decrease, handles overflowing lines into *
 * the next chunk, or deleting the current chunk. Returns a valid chunk in    *
 * case the one passed to it was free'd                                       */
static chunk *buffer_chunk_resize_bigger(chunk *c);
static chunk *buffer_chunk_resize_smaller(chunk *c);

/* Free a chunk without freeing the rest of the chunks in its chain */
static void buffer_chunk_free_norecurse(chunk *c);

chunk *buffer_chunk_init(void)
{
    chunk *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(chunk)), terminal,
               return NULL);

    /* We need to initialize a vector on the pointer we already *
     * allocated.                                               */
    ASSERT_PTR(vec_lines_create((vec_lines *)rtn), critical,
               return NULL);

    /* NULL to show that these are ends of the chain */
    rtn->next = NULL;
    rtn->prev = NULL;

    rtn->startline = 0;

    return rtn;
}

void buffer_chunk_free(chunk *c)
{
    chunk *curr, *next;

    if (!c) return;

    /* Go forward to the end of the chain, freeing all *
     * chunks along the way.                           */
    curr = c->next;
    while (curr)
    {
        next = curr->next;
        buffer_chunk_free_norecurse(curr);
        curr = next;
    }

    /* Go backwards and free those too */
    curr = c->prev;
    while (curr)
    {
        next = curr->prev;
        buffer_chunk_free_norecurse(curr);
        curr = next;
    }

    /* Finally, free the chunk we were handed */
    buffer_chunk_free_norecurse(c);
}

static void buffer_chunk_free_norecurse(chunk *c)
{
    /* Free every line, one by one */
    vec_foreach((vec *)c, line *, l,
                buffer_line_free(l));

    /* Free the vector that contained them */
    vec_lines_free((vec_lines *)c);
}

static void buffer_chunk_correct_startlines(chunk *c)
{
    lineno currstart;

    if (c->prev)
        /* Add the previous chunk's size to its start to get this *
         * chunk's start.                                         */
        currstart = c->prev->startline + vec_lines_len((vec_lines *)c->prev);
    else
        /* Or if it doesn't exist, this is the 1st chunk, so its *
         * start is 0!                                           */
        currstart = 0;

    do
    {
        /* Sum up the sizes of all the chunks, and set their *
         * startlines to the sum of previous chunks          */
        c->startline = currstart;
        currstart += vec_lines_len((vec_lines *)c);
        c = c->next;
    }
    while (c);
}

static chunk *buffer_chunk_insert(chunk *c)
{
    chunk *rtn;

    rtn = buffer_chunk_init();

    TRACE_PTR(rtn, return NULL);

    /* Insert the new chunk after the one we already have, by *
     * sewing it in.                                          */
    rtn->next = c->next;
    rtn->prev = c;

    /* Tell the next chunks about the new chunk behind them. */
    if (c->next)
        c->next->prev = rtn;

    c->next   = rtn;

    rtn->startline = c->startline + vec_lines_len((vec_lines *)c);

    return rtn;
}

static void buffer_chunk_delete(chunk *c)
{
    if (!c) return;

    /* If there's a next chunk, tell it that we *
     * deleted this chunk.                      */
    if (c->next)
        c->next->prev = c->prev;

    /* If there's a previous chunk, tell it that we *
     * deleted this chunk.                          */
    if (c->prev)
        c->prev->next = c->next;

    /* Free the chunk without affecting the others. */
    buffer_chunk_free_norecurse(c);
}

static chunk *buffer_chunk_insert_lines(chunk *c, size_t index, size_t n, line **lines)
{
    ASSERT_INT(vec_lines_insert((vec_lines *)c, index, n, lines),
               high, return NULL);

    return buffer_chunk_resize_bigger(c);
}

static chunk *buffer_chunk_resize_bigger(chunk *c)
{
    size_t     amount, len;
    chunk     *next;
    line     **iter;

    len = vec_lines_len((vec_lines *)c);

    /* Abandon if the chunk is a sane size */
    if (len <= BUFFER_CHUNK_MAX_SIZE)
        return c;

    if (c->next)
        next = c->next;
    else
        /* Make a new chunk if we're at the end */
        TRACE_PTR(next = buffer_chunk_insert(c), return NULL);

    /* Get the amount we need to overflow, and a pointer to the *
     * start of the data we're going to overflow                */
    amount = len - BUFFER_CHUNK_DEF_SIZE;
    iter   = vec_lines_item((vec_lines *)c, BUFFER_CHUNK_DEF_SIZE);

    /* Check that the pointer to data to overflow is valid */
    ASSERT_PTR(iter, critical,
               return NULL);

    /* Insert from that pointer if it is */
    buffer_chunk_insert_lines(next, 0, amount, iter);

    /* Delete lines from the chunk we overflowed from */
    ASSERT_INT(vec_lines_delete((vec_lines *)c, BUFFER_CHUNK_DEF_SIZE, amount),
               critical, return NULL);

    /* Resize subsequent chunks */
    if (next)
        buffer_chunk_resize_bigger(next);

    return c;
}

static chunk *buffer_chunk_resize_smaller(chunk *c)
{
    chunk     *next;
    line     **iter;
    size_t     len;

    len = vec_lines_len((vec_lines *)c);

    next = c->next;

    /* If chunk size is sane, return */
    if (len >= BUFFER_CHUNK_MIN_SIZE)
        return c;

    /* Also return if there's no next chunk.  *
     * The last chunk is allowed to be small. */
    if (next == NULL)
        return c;

    iter = vec_lines_item((vec_lines *)c, 0);

    ASSERT_PTR(iter, critical,
               return NULL);

    /* Insert all of the lines from our current chunk *
     * into the next chunk.                           */
    buffer_chunk_insert_lines(next, 0, len, iter);

    /* Delete the current chunk */
    buffer_chunk_delete(c);

    /* Resize the next chunk, which has now grown, *
     * and return a new valid chunk.               */
    return buffer_chunk_resize_bigger(next);
}

int buffer_chunk_insert_line(chunk *c, lineno offset)
{
    line  *l;

    /* Make a new line */
    l = buffer_line_init();

    TRACE_PTR(l, return -1);

    /* Insert the line */
    TRACE_PTR(buffer_chunk_insert_lines(c, offset, 1, &l),
              return -1);

    /* Resize the chunk */
    TRACE_PTR(buffer_chunk_resize_bigger(c),
              return -1);

    /* That's it really... */
    buffer_chunk_correct_startlines(c);

    return 0;
}

chunk *buffer_chunk_delete_line(chunk *c, lineno offset)
{
    line  *l;

    l = vec_lines_get((vec_lines *)c, offset);

    ASSERT_PTR(l, high, return NULL);

    /* Delete the line from the chunk */
    ASSERT_INT(vec_lines_delete((vec_lines *)c, offset, 1),
               critical, return NULL);

    /* Free the actual line */
    buffer_line_free(l);

    /* Resize the chunk */
    ASSERT_PTR(c = buffer_chunk_resize_smaller(c),
               critical, return NULL);

    buffer_chunk_correct_startlines(c);

    /* Return a valid chunk */
    return c;
}

vec *buffer_chunk_get_line(chunk *c, lineno offset)
{
    vec *rtn;

    /* Get a vector from a line */
    rtn = buffer_line_get_vec(vec_lines_get((vec_lines *)c, offset));

    TRACE_PTR(rtn, return NULL);

    return rtn;
}

int buffer_chunk_set_line(chunk *c, lineno offset, vec *v)
{
    /* Set a line's contents to a vector */
    TRACE_INT(buffer_line_set_vec(vec_lines_get((vec_lines *)c, offset), v),
               return -1);

    return 0;
}

chunk *buffer_chunk_get_containing(chunk *c, lineno ln)
{
    ASSERT_PTR(c, high, return NULL);

    /* While we're too far forwards, iterate backwards */
    while (ln < c->startline && c->prev)
        c = c->prev;

    /* While we're too far backwards, iterate forwards */
    while (ln >= c->startline + vec_lines_len((vec_lines *)c) && c->next)
        c = c->next;

    return c;
}

lineno buffer_chunk_lineno_to_offset(chunk *c, lineno ln)
{
    ASSERT_PTR(c, high, return 0);

    return ln - c->startline;
}

lineno buffer_chunk_offset_to_lineno(chunk *c, lineno offset)
{
    ASSERT_PTR(c, high, return 0);

    return offset + c->startline;
}

lineno buffer_chunk_get_total_len(chunk *c)
{
    ASSERT_PTR(c, high, return 0);

    /* Get the final chunk */
    while (c->next)
        c = c->next;

    /* Return its last line */
    return c->startline + vec_lines_len((vec_lines *)c);
}
