#include <stdlib.h>
#include <string.h>

#include "container/vec.h"

vecerr_t vecerr;

static int vec_realloc(vec *v);
static int vec_resize_bigger(vec *v);
static int vec_resize_smaller(vec *v);

/* Set vecerr to an error with suffix err and return rtn */
#define ERR(err, rtn) {vecerr = E_VEC_ ## err; return rtn;}

/* Functions for pointer arithmetic */
#define addptr(ptr, n) (void *)((intptr_t)(ptr) + (ptrdiff_t)(n))
#define subptr(ptr, n) (void *)((intptr_t)(ptr) - (ptrdiff_t)(n))

static int vec_realloc(vec *v)
{
    v->data = realloc(v->data, v->capacity);

    /* Check allocs :D */
    if (v->data == NULL)
    {
        /* Set the capacity and length to 0 to try and prevent *
         * anything segfaulting off of this                    */
        v->length   = 0;
        v->capacity = 0;

        ERR(NO_MEMORY, -1);
    }

    ERR(OK, 0);
}

static int vec_resize_bigger(vec *v)
{
    size_t length;

    length = v->length;

    /* If we have enough capacity we can just return */
    if (v->capacity >= length)
        ERR(OK, 0);

    /* If due to errors, we get a capacity of 0, *
     * set it to the minimum allowable value     */
    if (v->capacity < v->width)
        v->capacity = v->width;

    /* If we don't have capacity, double it, and if  *
     * that STILL isn't enough, keep doing it.       */
    while (v->capacity < length)
        v->capacity <<= 1;

    /* Now that we've changed v->capacity, we gotta realloc */
    return vec_realloc(v);
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
        ERR(OK, 0);

    /* If we need to shrink, halve the capacity, and if *
     * we still need shrink, keep doing it              */
    do
        v->capacity >>= 1;
    while (length < v->capacity >> 2  &&
           width  < v->capacity);

    /* Now that we've changed v->capacity, we gotta realloc */
    return vec_realloc(v);
}

static vec *vec_init_raw(size_t width)
{
    vec *rtn;

    /* We cannot have zero width vectors */
    if (width == 0)
        ERR(INVALID_WIDTH, NULL);

    rtn = malloc(sizeof(vec));

    /* Check allocated struct vec_s */
    if (rtn == NULL)
        ERR(NO_MEMORY, NULL);

    /* Data must be null so realloc will malloc */
    rtn->data     = NULL;
    rtn->width    = width;
    rtn->length   = 0;
    rtn->capacity = 0;

    ERR(OK, rtn);
}

vec *vec_init(size_t width)
{
    vec *rtn;

    rtn = vec_init_raw(width);

    if (rtn == NULL)
        return NULL;

    /* We set our capacity to the vector's     *
     * width (one item) and allocate it.       *
     * Vector's capacities are always at least *
     * their width                             */
    if (vec_resize_bigger(rtn) != 0)
    {
        free(rtn);
        return NULL;
    }

    ERR(OK, rtn);
}

void vec_free(vec *v)
{
    free(v->data);
    free(v);
}

void *vec_item(vec *v, size_t index)
{
    size_t offset;

    /* Check yo pointers */
    if (v == NULL)
        ERR(NULL_VEC, NULL);

    /* Calculate and copy to offset */
    offset = index * v->width;

    /* Check that index is in bounds */
    if (offset + v->width > v->length)
        ERR(INVALID_INDEX, NULL);

    return addptr(v->data, offset);
}

int vec_delete(vec *v, size_t index, size_t n)
{
    size_t offset, amount;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    /* Calculate the offset of the startbyte of the     *
     * deletion, and the amount of bytes we will delete */
    offset = index * v->width;
    amount =     n * v->width;

    /* Check that the range to delete isn't out of bounds */
    if (offset + amount > v->length)
        ERR(INVALID_INDEX, -1);

    /* Move memory beyond offset backwards by amount. */
    memmove(addptr(v->data, offset),
            addptr(v->data, offset + amount),
            v->length - offset - amount);

    /* Modify length and resize */
    v->length -= amount;

    return vec_resize_smaller(v);
}

int vec_insert(vec *v, size_t index, size_t n, const void *new)
{
    size_t offset, amount, displaced;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    if (new == NULL)
        ERR(NULL_VALUE, -1);

    /* Calculate the offset of the startbyte of the       *
     * insertion, and the amount of bytes we will insert, *
     * and the size of the region displaced forward.      */
    offset = index * v->width;
    amount =     n * v->width;
    displaced = v->length - offset;

    /* Make sure we're in bounds */
    if (offset > v->length)
        ERR(INVALID_INDEX, -1);

    /* We adjust length and realloc before moving anything */
    v->length += amount;

    if (vec_resize_bigger(v) == -1)
        return -1;

    /* If any data is displaced, we move it forward to make *
     * room for inserted data.                              */
    if (displaced)
        memmove(addptr(v->data, offset + amount),
                addptr(v->data, offset),
                displaced);

    /* We insert the new data */
    memmove(addptr(v->data, offset),
            new, amount);

    ERR(OK, 0);
}

size_t vec_len(vec *v)
{
    if (v == NULL)
        ERR(NULL_VEC, 0);

    /* To get the number of items, divide *
     * length in bytes by width in bytes  */
    ERR(OK, v->length / v->width);
}

size_t vec_find(vec *v, const void *item)
{
    void *cmp, *last;
    size_t width, index;

    if (v == NULL)
        ERR(NULL_VEC, INVALID_INDEX);

    if (item == NULL)
        ERR(NULL_VALUE, INVALID_INDEX);

    /* Get the first item of the list, and *
     * the one just past the end (invalid) */
    width  = v->width;
    cmp    = v->data;
    last   = addptr(cmp, v->length);


    index  = 0;
    while (cmp != last)
    {
        /* If there's a match, return the index */
        if (memcmp(cmp, item, width) == 0)
            ERR(OK, index);

        /* Increment cmp by width, and index by 1 */
        cmp = addptr(cmp, width);
        ++index;
    }

    /* No match */
    ERR(INVALID_VALUE, INVALID_INDEX);
}

size_t vec_rfind(vec *v, const void *item)
{
    void *cmp, *first;
    size_t width, index;

    /* Check the values */
    if (v == NULL)
        ERR(NULL_VEC, INVALID_INDEX);

    if (item == NULL)
        ERR(NULL_VALUE, INVALID_INDEX);

    /* Get the first item of the list, and *
     * the one just past the end (invalid) */
    width  = v->width;
    first  = v->data;
    cmp    = addptr(first, v->length);

    /* While we're not at the start */
    index  = vec_len(v);
    while (cmp > first)
    {
        /* Decrement cmp by width and index by 1 */
        index--;
        cmp = subptr(cmp, width);

        /* If there's a match, return the index */
        if (memcmp(cmp, item, width) == 0)
            ERR(OK, index);
    }

    /* No match */
    ERR(INVALID_VALUE, INVALID_INDEX);
}

/* Yes I can make this from the others, but fuck it, *
 * this is way faster...                             */
vec *vec_cut(vec *v, size_t index, size_t n)
{
    void  *slice;
    vec   *rtn;
    size_t width, offset, amount;

    if (v == NULL)
        ERR(NULL_VEC, NULL);

    width = v->width;

    /* Offset is the offset of the startindex, *
     * amount is the amount in bytes to cut    */
    offset = index * width;
    amount =     n * width;

    /* Ensure we have a valid range */
    if (offset + amount > v->length)
        ERR(INVALID_INDEX, NULL);

    /* Starting point for cut */
    slice = addptr(v->data, offset);

    /* Make a new vector for the cut */
    rtn = vec_init_raw(width);
    rtn->length = n;

    if (vec_resize_bigger(rtn) != 0)
    {
        vec_free(rtn);
        return NULL;
    }

    /* Copy the data for the cut to the new vector */
    memcpy(rtn->data, slice, amount);

    return rtn;
}

const char *vec_err_str(void)
{
    if (vecerr == E_VEC_NULL_VALUE)
        return "E_VEC_NULL_VALUE";

    if (vecerr == E_VEC_NULL_VEC)
        return "E_VEC_NULL_VEC";

    if (vecerr == E_VEC_INVALID_INDEX)
        return "E_VEC_INVALID_INDEX";

    if (vecerr == E_VEC_INVALID_VALUE)
        return "E_VEC_INVALID_VALUE";

    if (vecerr == E_VEC_NO_MEMORY)
        return "E_VEC_NO_MEMORY";

    if (vecerr == E_VEC_INVALID_WIDTH)
        return "E_VEC_INVALID_WIDTH";

    /* Great error. */
    if (vecerr == E_VEC_OK)
        return "E_VEC_OK";

    return "<Unknown vec error>";
}
