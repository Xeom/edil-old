#include <stdlib.h>
#include <string.h>

#include "err.h"

#include "container/vec.h"

static int vec_realloc(vec *v);
static int vec_resize_bigger(vec *v);
static int vec_resize_smaller(vec *v);

static int vec_realloc(vec *v)
{
    v->data = realloc(v->data, v->capacity);

    /* Check allocs :D */

    ASSERT_PTR(v->data, terminal,
               /* Set the capacity and length to 0 to try and *
                * prevent anything segfaulting off of this    */
               v->length   = 0;
               v->capacity = 0;
               return -1;
        );

    return 0;
}

static int vec_resize_bigger(vec *v)
{
    size_t length;

    length = v->length;

    /* If we have enough capacity we can just return */
    if (v->capacity >= length)
        return 0;

    /* If the vec is new, we get a capacity of 0, *
     * set it to the minimum allowable value.     */
    if (v->capacity < v->width)
        v->capacity = v->width;

    /* If we don't have capacity, double it, and if  *
     * that STILL isn't enough, keep doing it.       */
    while (v->capacity < length)
        v->capacity <<= 1;

    /* Now that we've changed v->capacity, we gotta realloc */
    TRACE_INT(vec_realloc(v), return -1);

    return 0;
}

static int vec_resize_smaller(vec *v)
{
    size_t width, length;

    length = v->length;
    width  = v->width;

    /* If we are using over one quarter capacity, or *
     * our capacity is at minimum already, return    */
    if (length >= v->capacity >> 2 ||
        width  == v->capacity)
        return 0;

    /* If we need to shrink, halve the capacity, and if *
     * we still need shrink, keep doing it              */
    do
        v->capacity >>= 1;
    while (length < v->capacity >> 2  &&
           width  < v->capacity);

    /* Now that we've changed v->capacity, we gotta realloc */
    TRACE_INT(vec_realloc(v), return -1);

    return 0;
}

static vec *vec_init_raw(size_t width)
{
    vec *rtn;

    /* We cannot have zero width vectors */
    ASSERT(width != 0, high, return NULL);

    ASSERT_PTR(rtn = malloc(sizeof(vec)),
               high, return NULL);

    ASSERT_PTR(vec_create(rtn, width),
               high, return NULL);

    return rtn;
}

vec *vec_create(void *mem, size_t width)
{
    vec *rtn;

    rtn = (vec *)mem;

    rtn->data     = NULL;
    rtn->width    = width;
    rtn->length   = 0;
    rtn->capacity = 0;

    return rtn;
}

vec *vec_init(size_t width)
{
    vec *rtn;

    TRACE_PTR(rtn = vec_init_raw(width), return NULL);

    TRACE_INT(vec_resize_bigger(rtn),
              free(rtn);
              return NULL;
        );

    return rtn;
}

void vec_free(vec *v)
{
    if (!v) return;

    vec_kill(v);
    free(v);
}

void vec_kill(vec *v)
{
    if (!v) return;

    free(v->data);
}

void *vec_item(vec *v, size_t index)
{
    size_t offset;

    /* Check yo pointers */
    ASSERT_PTR(v, high, return NULL);

    /* Calculate and copy to offset */
    offset = index * v->width;

    /* Check that index is in bounds */
    ASSERT(offset + v->width <= v->length,
           high, return NULL);

    return ADDPTR(v->data, offset);
}

int vec_delete(vec *v, size_t index, size_t n)
{
    size_t offset, amount;

    ASSERT_PTR(v, high, return -1);

    /* Calculate the offset of the startbyte of the     *
     * deletion, and the amount of bytes we will delete */
    offset = index * v->width;
    amount =     n * v->width;

    /* Check that the range to delete isn't out of bounds */
    ASSERT(offset + amount <= v->length, high, return -1);

    /* Move memory beyond offset backwards by amount. */
    memmove(ADDPTR(v->data, offset),
            ADDPTR(v->data, offset + amount),
            v->length - offset - amount);

    /* Modify length and resize */
    v->length -= amount;

    TRACE_INT(vec_resize_smaller(v), return -1);

    return 0;
}
#include <unistd.h>
int vec_insert(vec *v, size_t index, size_t n, const void *new)
{
    size_t offset, amount, displaced;

    ASSERT_PTR(v, high, return -1);

    /* Calculate the offset of the startbyte of the       *
     * insertion, and the amount of bytes we will insert, *
     * and the size of the region displaced forward.      */
    offset = index * v->width;
    amount =     n * v->width;
    displaced = v->length - offset;

    /* Make sure we're in bounds */
    ASSERT(offset <= v->length,
           high, return -1);

    /* We adjust length and realloc before moving anything */
    v->length += amount;

    TRACE_INT(vec_resize_bigger(v),
              return -1);

    /* If any data is displaced, we move it forward to make *
     * room for inserted data.                              */
    if (displaced)
        memmove(ADDPTR(v->data, offset + amount),
                ADDPTR(v->data, offset),
                displaced);

    if (new)
        /* We insert the new data */
        memcpy(ADDPTR(v->data, offset),
               new, amount);
    else
        /* If it's null, we just set to 0.. */
        memset(ADDPTR(v->data, offset),
               0, amount);

    return 0;
}

size_t vec_len(vec *v)
{
    size_t *a;
    a = NULL;
    ASSERT_PTR(v, high, return *a);
    ASSERT(v->width != 0, critical, return 0);

    /* To get the number of items, divide *
     * length in bytes by width in bytes  */
    return v->length / v->width;
}

size_t vec_find(vec *v, const void *item)
{
    void *cmp, *last;
    size_t width, index;

    /* Check the values */
    ASSERT_PTR(v,    high, return INVALID_INDEX);
    ASSERT_PTR(item, high, return INVALID_INDEX);

    /* Get the first item of the list, and *
     * the one just past the end (invalid) */
    width  = v->width;
    cmp    = v->data;
    last   = ADDPTR(cmp, v->length);

    index  = 0;
    while (cmp != last)
    {
        /* If there's a match, return the index */
        if (memcmp(cmp, item, width) == 0)
            return index;

        /* Increment cmp by width, and index by 1 */
        cmp = ADDPTR(cmp, width);
        ++index;
    }

    /* No match */
    ERR_NEW(high, "Invalid value", "The value is not in the vec");

    return INVALID_INDEX;
}

size_t vec_rfind(vec *v, const void *item)
{
    void *cmp, *first;
    size_t width, index;

    /* Check the values */
    ASSERT_PTR(v,    high, return INVALID_INDEX);
    ASSERT_PTR(item, high, return INVALID_INDEX);

    /* Get the first item of the list, and *
     * the one just past the end (invalid) */
    width  = v->width;
    first  = v->data;
    cmp    = ADDPTR(first, v->length);

    /* While we're not at the start */
    index  = vec_len(v);
    while (cmp > first)
    {
        /* Decrement cmp by width and index by 1 */
        index--;
        cmp = SUBPTR(cmp, width);

        /* If there's a match, return the index */
        if (memcmp(cmp, item, width) == 0)
            return index;
    }

    /* No match */
    ERR_NEW(high, "Invalid value", "The value is not in the vec");

    return INVALID_INDEX;
}

int vec_contains(vec *v, const void *item)
{
    void *cmp, *last;
    size_t width;

    /* Check the values */
    ASSERT_PTR(v,    high, return -1);
    ASSERT_PTR(item, high, return -1);

    /* Get the first item of the list, and *
     * the one just past the end (invalid) */
    width  = v->width;
    cmp    = v->data;
    last   = ADDPTR(cmp, v->length);

    while (cmp != last)
    {
        /* If there's a match, return 1 */
        if (memcmp(cmp, item, width) == 0)
            return 1;

        /* Increment cmp by width */
        cmp = ADDPTR(cmp, width);
    }

    /* No match */
    return 0;
}

/* Yes I can make this from the others, but fuck it, *
 * this is way faster...                             */
vec *vec_cut(vec *v, size_t index, size_t n)
{
    void  *slice;
    vec   *rtn;
    size_t width, offset, amount;

    ASSERT_PTR(v, high, return NULL);

    width = v->width;

    /* Offset is the offset of the startindex, *
     * amount is the amount in bytes to cut    */
    offset = index * width;
    amount =     n * width;

    /* Ensure we have a valid range */
    ASSERT(offset + amount <= v->length, high, return NULL);

    /* Starting point for cut */
    slice = ADDPTR(v->data, offset);

    /* Make a new vector for the cut */
    rtn = vec_init_raw(width);
    rtn->length = n;

    TRACE_INT(vec_resize_bigger(rtn),
              vec_free(rtn);
              return NULL);

    /* Copy the data for the cut to the new vector */
    memcpy(rtn->data, slice, amount);

    return rtn;
}
