#include <stdlib.h>
#include <string.h>

#include "err.h"

#include "container/vec.h"

/*
 * Realloc a vec to its current capacity. Used when a vec is resized - its
 * capacity property is changed, then this function is called to realloc its
 * data.
 *
 * @param v A pointer to the vector to realloc.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int vec_realloc(vec *v);

/*
 * Perform any appropriate or necessary resizing after the length of a vec has
 * increased. This function must be called after increasing the length property
 * of a vec. If this function succeeds, the vec can then be assumed to have
 * sufficient capacity for whatever is put in it.
 *
 * @param v A pointer to the vec to enlarge.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int vec_resize_bigger(vec *v);

/*
 * Perform any appropriate resizing on avec after its length has decreased.
 *
 * @param v A pointer to the vec that is shrinking.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int vec_resize_smaller(vec *v);

/*
 * Compare a value in a vec with a pointer to some memory. Used in bisearch.
 * This function either works the same as memcmp, or in reverse, depending on
 * the endianness of the system. If the value at index in the vector is greater
 * than item, a positive value is returned. If they are equal, 0 is returned,
 * a negative number is returned if item is greater.
 *
 * @param v      A pointer to the vector to take an item from and compare.
 * @param index  The index of the item in the vector to compare.
 * @param item   A pointer to an item to compare to the vector's item.
 * @param map    A series of bytes, the same length as the a value in v. Acts as
 *               a bitmask for the comparison - 0s are ignored.
 *
 */
static inline int vec_bisearch_compare(
    vec *v, size_t index, const uchar *item, const uchar *map);


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
    ASSERT_PTR(v, high, return 0);
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

static inline int vec_bisearch_compare(
    vec *v, size_t index, const uchar *item, const uchar *map)
{
    size_t width, byte;
    uchar  *vecitem;

    vecitem = vec_item(v, index);
    width   = v->width;

#if defined(BIG_ENDIAN)
    /* If big endian, start at the beginning */
    byte = 0;
#elif defined(LITTLE_ENDIAN)
    /* If little endian, start at the end */
    byte = v->width - 1;
#endif

    while (width--)
    {
        int diff;
        if (map[byte])
        {
            diff  = vecitem[byte] & map[byte];
            diff -= item[byte]    & map[byte];

            if (diff) return diff;
        }

        /* Move either forward or backwards depending on endianness */
#if defined(BIG_ENDIAN)
        ++byte;
#elif defined(LITTLE_ENDIAN)
        --byte;
#endif
    }

    return 0;
}

size_t vec_bisearch(vec *v, const void *item, const void *map)
{
    /* These are the lowest and highest indices we could be looking for.  */
    size_t upper, lower;

    ASSERT_PTR(v,    high, return INVALID_INDEX);
    ASSERT_PTR(item, high, return INVALID_INDEX);
    ASSERT_PTR(map,  high, return INVALID_INDEX);

    /* Put the bounds at each extreme of the vector. */
    lower = 0;
    upper = vec_len(v);

    /* Repeat until both bounds converge. */
    while (upper != lower)
    {
        size_t pivot;

        /* Take a position halfway through the vector.              *
         * Be sure to round down, otherwise infinite loops happen.  */
        pivot = (upper) / 2 + (lower) / 2;

        /* If the pivot is greater or equal to the item we're looking for ... */
        if (vec_bisearch_compare(v, pivot, item, map) >= 0)
            /* ... set a new upper bound to the pivot's value */
            upper = pivot;
        else
            /* If the pivot value is smaller than the item we're looking for, *
             * we know the pivot cannot be the correct value, so our new      *
             * lower bound is one position ahead of the pivot.                */
            lower = pivot + 1;
    }

    return upper;
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
    rtn->length = amount;

    TRACE_INT(vec_resize_bigger(rtn),
              vec_free(rtn);
              return NULL);

    /* Copy the data for the cut to the new vector */
    memcpy(rtn->data, slice, amount);

    return rtn;
}
