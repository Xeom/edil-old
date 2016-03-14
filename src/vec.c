#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "vec.h"

#define ERR(err, rtn) {vecerr = E_VEC_ ## err; return rtn;}

vecerr_t vecerr;

static int vec_realloc(vec *v);
static int vec_resize_bigger(vec *v);
static int vec_resize_smaller(vec *v);

static int vec_realloc(vec *v)
{
    v->data = realloc(v->data, v->capacity);

    if (v->data == NULL)
        ERR(NO_MEMORY, -1);

    ERR(OK, 0);
}

static int vec_resize_bigger(vec *v)
{
    size_t length;

    length = v->length;

    if (v->capacity >= length)
        ERR(OK, 0);

    do
        v->capacity <<= 1;
    while (v->capacity < length);

    return vec_realloc(v);
}

static int vec_resize_smaller(vec *v)
{
    size_t length, width;

    length = v->length;
    width  = v->width;

    if (length <= v->capacity >> 2 ||
        width  == v->capacity)
        ERR(OK, 0);

    do
        v->capacity >>= 1;
    while (length > v->capacity &&
           width  < v->capacity);

    return vec_realloc(v);
}

vec *vec_init(size_t width)
{
    vec *rtn;

    /* We cannot have zero width vectors */
    if (width == 0)
        ERR(INVALID_WIDTH, NULL);

    rtn = malloc(sizeof(vec));

    /* Check allocated struct vec_s */
    if (rtn == NULL)
        ERR(NO_MEMORY, NULL);

    rtn->width    = width;
    rtn->length   = 0;
    rtn->capacity = width;

    /* We set our capacity to the vector's     *
     * width (one item) and allocate it.       *
     * Vector's capacities are always at least *
     * their width                             */
    if (vec_realloc(rtn) != 0)
        return NULL;

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

    if (offset + v->width > vec->length)
        ERR(INVALID_INDEX, NULL);

    return v->data + offset;
}

int vec_delete(vec *v, size_t index, size_t n)
{
    size_t offset, amount;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    offset = index * v->width;
    amount =     n * v->width;

    if (offset + amount > vec->length)
        ERR(INVALID_INDEX, -1);

    memmove(v->data + offset,
            v->data + offset + amount,
            v->length - offset);

    v->length -= amount;

    return vec_resize_smaller(v);
}

int vec_insert(vec *v, size_t index, size_t n, const void *new)
{
    size_t offset, amount;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    if (new == NULL)
        ERR(NULL_VALUE, -1);

    offset = index * v->width;
    amount =     n * v->width;

    v->length += amount;

    if (vec_resize_bigger(v) == -1)
        return -1;

    memmove(v->data + offset + amount,
            v->data + offset,
            v->length - offset);

    memmove(v->data + offset,
            new, amount);

    ERR(OK, 0);
}

size_t vec_len(vec *v)
{
    if (v == NULL)
        ERR(NULL_VEC, 0);

    ERR(OK, v->length / v->width);
}

size_t vec_find(vec *v, const void *item)
{
    void *cmp, *last;
    size_t width, index;

    if (v == NULL)
        ERR(NULL_VEC, INVALID_INDEX);

    if (item == NULL)
        ERR(NULL_VAULE, INVALID_INDEX);

    width  = v->width;
    cmp    = v->data;
    last   = cmp + v->length;

    index  = 0;
    while (cmp != last)
    {
        if (memcmp(cmp, item, width) == 0)
            ERR(OK, i);
        cmp += width;
        ++index;
    }

    ERR(INVALID_VALUE, INVALID_INDEX);
}

size_t vec_rfind(vec *v, const void *item)
{
    void *cmp, *first;
    size_t width, index;

    if (v == NULL)
        ERR(NULL_VEC, INVALID_INDEX);

    if (item == NULL)
        ERR(NULL_VAULE, INVALID_INDEX);

    width  = v->width;
    first  = v->data;
    cmp    = first + v->length;

    index  = vec_len(v);
    while (cmp > first)
    {
        index--;
        cmp -= width;

        if (memcmp(cmp, item, width) == 0)
            ERR(OK, index);
    }

    ERR(INVALID_VALUE, INVALID_INDEX);
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

    if (vecerr == E_VEC_OK)
        return "E_VEC_OK";

    return "<Unknown vec error>";
}
