#include <stdlib.h>
#include "chunk.h"
#include "err.h"

struct chunk_s
{
    size_t startindex;
    chunk *next;
    chunk *prev;
    vec   *items;
};

#define CHUNK_DEFAULT  128
#define CHUNK_MIN      (CHUNK_DEFAULT << 2)
#define CHUNK_MAX      (CHUNK_DEFAULT + (128 << 2))

int chunk_items_set_chunk(chunk *c, vec *items);

int chunk_items_set_chunk(chunk *c, vec *items)
{
    vec_foreach(items, chunk **, curr,
                *curr = c;
        );

    return 0;
}

int chunk_insert_item(chunk *c, size_t offset, void *item)
{
    CHECK_NULL_PRM(chunk_insert_item, item, -1);

    if (vec_insert(c->items, offset, 1, &item))
    {
        if (vecerr == E_VEC_INVALID_INDEX)
            ERR_NEW(high, "chunk_insert_item: Invalid offset handed to function", NULL);

        else
            ERR_NEW(critical, "chunk_insert_item: Inserting into items vector failed", vec_err_str());

        return -1;
    }

    TRACE_NONZRO_CALL(textcont_insert, chunk_resize(c), -1);

    chunk_reset_starts(c);

    return 0;
}

int chunk_remove_item(chunk *c, void *item)
{
    size_t index;

    CHECK_NULL_PRM(chunk_remove_line, c, -1);

    index = vec_find(c->items, &item);

    if (index == INVALID_INDEX)
    {
        ERR_NEW(critical, "chunk_remove_item: Removing item from vector failed", vec_err_str());
        return -1;
    }

    vec_delete(c->items, index, 1);

    TRACE_NONZRO_CALL(chunk_remove_line, chunk_resize(c), -1);
    chunk_reset_starts(c);

    return 0;
}

size_t chunk_get_startindex(chunk *c)
{
    CHECK_NULL_PRM(chunk_get_startline, c, INVALID_INDEX);

    return c->startindex;
}

size_t chunk_find_item_offset(chunk *c, void *item)
{
    size_t index;

    CHECK_NULL_PRM(chunk_find_item, c, INVALID_INDEX);

    index = vec_find(c->items, &item);

    if (index == INVALID_INDEX)
    {
        ERR_NEW(critical, "chunk_find_item_offset: Finding item in vector failed", vec_err_str());
        return INVALID_INDEX;
    }

    return index;
}

size_t chunk_rfind_item_offset(chunk *c, void *item)
{
    size_t index;

    CHECK_NULL_PRM(chunk_find_item, c, INVALID_INDEX);

    index = vec_rfind(c->items, &item);

    if (index == INVALID_INDEX)
    {
        ERR_NEW(critical, "chunk_rfind_item_offset: Finding item in vector failed", vec_err_str());
        return INVALID_INDEX;
    }

    return index;
}

void *chunk_get_item(chunk *c, size_t i)
{
    void **rtn;

    CHECK_NULL_PRM(chunk_get_item, c, NULL);

    rtn = vec_item(c->items, i);

    TRACE_NULL(chunk_get_item, vec_get(c->items, i), rtn, NULL);

    return *rtn;
}

size_t chunk_len(chunk *c)
{
    CHECK_NULL_PRM(chunk_len, c, 0);

    return vec_len(c->items);
}

chunk *chunk_prev(chunk *c)
{
    CHECK_NULL_PRM(chunk_prev, c, NULL);

    return c->prev;
}

chunk *chunk_next(chunk *c)
{
    CHECK_NULL_PRM(chunk_prev, c, NULL);

    return c->next;
}

int chunk_is_first(chunk *c)
{
    return c->next == NULL;
}

int chunk_is_last(chunk *c)
{
    return c->prev == NULL;
}

chunk *chunk_init(void)
{
    chunk *rtn;

    rtn = malloc(sizeof(chunk));

    CHECK_ALLOC(chunk_init, rtn, NULL);

    rtn->startindex = 0;
    rtn->next       = NULL;
    rtn->prev       = NULL;
    rtn->items      = vec_init(sizeof(void*));

    if (rtn->items == NULL)
    {
        free(rtn);
        TRACE(chunk_init, vec_init(sizeof(void*)), NULL);
    }
    
    return rtn;
}

void chunk_free(chunk *c)
{
    if (c->prev)
        c->prev->next = c->next;

    if (c->next)
        c->next->prev = c->prev;

    vec_free(c->items);
    free(c);
}

void chunk_reset_starts(chunk *c)
{
    size_t newstart;

    while (c->next)
    {
        newstart = c->startindex + vec_len(c->items);
        c->next->startindex = newstart;
        c = c->next;
    }
}

int chunk_resize(chunk *c)
{
    chunk *dump;
    vec   *overflow;

    if (vec_len(c->items) > CHUNK_MAX)
    {
        size_t slice_i, slice_n;
        slice_i = CHUNK_DEFAULT;
        slice_n = vec_len(c->items) - CHUNK_DEFAULT;

        overflow = vec_cut(c->items, slice_i, slice_n);

        TRACE_NULL(chunk_resize, vec_cute, overflow, -1);

        TRACE_NONZRO_CALL(chunk_resize,
                          vec_delete(c->items, slice_i, slice_n), -1);

        if (c->next)
            dump = c->next;
        else
        {
            dump = chunk_init();

            if (dump == NULL)
            {
                vec_free(overflow);
                TRACE(chunk_resize, chunk_init(), -1);
            }

            dump->prev = c;
            c->next    = dump;
        }

        chunk_items_set_chunk(dump, overflow);

        TRACE_NONZRO_CALL(chunk_resize,
                          vec_insert_vec(dump->items, 0, overflow), -1);

        vec_free(overflow);

        TRACE_NONZRO_CALL(chunk_resize, chunk_resize(dump), -1);

        return 0;
    }

    if (vec_len(c->items) < CHUNK_MIN && c->next)
    {
        dump     = c->next;
        overflow = c->items;

        chunk_items_set_chunk(dump, overflow);

        TRACE_NONZRO_CALL(chunk_resize,
                          vec_insert_vec(dump->items, 0, overflow), -1);

        chunk_free(c);

        TRACE_NONZRO_CALL(chunk_resize, chunk_resize(dump), -1);

        return 0;
    }

    return 0;
}

chunk *chunk_find_containing(chunk *c, size_t n)
{
    while (c->startindex > n)
    {
        if (chunk_is_first(c))
        {
            ERR_NEW(critical, "chunk_find_containing: First chunk startline nonzero",
                    "Chunk with no prev link has a startline greater than requested line");
            return NULL;
        }
        c = c->prev;
    }

    while (chunk_get_startindex(c) + chunk_len(c) < n)
    {
        if (chunk_is_last(c))
        {
            ERR_NEW(medium, "chunk_find_containing: Line out of range of textcont", "");
            return NULL;
        }
        c = c->next;
    }

    return c;
}
