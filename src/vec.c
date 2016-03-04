#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "vec.h"

#define ERR(err, rtn) {vecerr = E_VEC_ ## err; return rtn;}

vecerr_t vecerr;

struct vec_s
{
    void *data;
    size_t width;
    size_t length;
    size_t capacity;
};

int vec_resize(vec *v);

int vec_resize(vec *v)
{
    if (v->length > v->capacity)
        v->capacity <<= 1;

    else if (v->length <  v->capacity >> 2 &&
             v->length >= v->width)
        v->capacity >>= 1;

    else
        return 0;

    v->data = realloc(v->data, v->capacity);

    if (v->data == NULL)
        ERR(NO_MEMORY, -1);

    return 0;
}

vec *vec_init(size_t width)
{
    vec *rtn;

    if (width == 0)
        ERR(INVALID_WIDTH, NULL);

    rtn = malloc(sizeof(vec));

    if (rtn == NULL)
        ERR(NO_MEMORY, NULL);

    rtn->data     = malloc(width);

    if (rtn->data == NULL)
        ERR(NO_MEMORY, NULL);

    rtn->width    = width;
    rtn->length   = 0;
    rtn->capacity = width;

    if (vec_resize(rtn) != 0)
        return NULL;

    return rtn;
}

void vec_free(vec *v)
{
    free(v->data);
    free(v);
}

int vec_push(vec *v, const void *value)
{
    ptrdiff_t offset;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    if (value == NULL)
        ERR(NULL_VALUE, -1);

    v->length += v->width;

    if (vec_resize(v) == -1)
        return -1;

    offset = v->length - v->width;

    memcpy((void *)((intptr_t)v->data + offset), value, v->width);

    return 0;
}

void *vec_pop(vec *v)
{
    void *rtn;
    ptrdiff_t offset;

    if (v == NULL)
        ERR(NULL_VEC, NULL);

    if (v->length == 0)
        ERR(INVALID_INDEX, NULL);

    rtn = malloc(v->width);

    if (rtn == NULL)
        ERR(NO_MEMORY, NULL);

    offset = v->length - v->width;

    memcpy(rtn, (void *)((intptr_t)v->data + offset), v->width);

    v->length -= v->width;

    if (vec_resize(v) != 0)
        return NULL;

    return 0;
}

void *vec_get(vec *v, size_t index)
{
    size_t offset = v->width * index;

    if (offset >= v->length)
        ERR(INVALID_INDEX, NULL);

    return (void *)((intptr_t)v->data + (ptrdiff_t)offset);
}

int vec_set(vec *v, size_t index, const void *data)
{
    size_t offset = v->width * index;

    if (offset >= v->length)
        ERR(INVALID_INDEX, -1);

    memcpy((void *)((intptr_t)v->data + (ptrdiff_t)offset),
           data, v->width);

    return 0;
}

int vec_trim(vec *v, size_t amount)
{
    size_t newlength;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    newlength = amount * v->length;

    if (newlength > v->length)
        ERR(INVALID_INDEX, -1);

    if (vec_resize(v) != 0)
        return -1;

    return 0;
}

vec *vec_slice(vec *v, size_t start, size_t end)
{
    vec *rtn;

    if (v == NULL)
        ERR(NULL_VEC, NULL);

    rtn = vec_init(v->width);

    if (rtn == NULL)
        return NULL;

    for (;start < end && start < v->length; ++start)
    {
        if (vec_push(rtn, vec_get(v, start)) != 0)
            return NULL;
    }

    return rtn;
}

int vec_append(vec *v, vec *other)
{
    size_t i;

    if (v == NULL || other == NULL)
        ERR(NULL_VEC, -1);

    i = 0;

    while (i < v->length)
    {
        if (vec_push(v, vec_get(other, i)) != 0)
            return -1;
        ++i;
    }

    return 0;
}

size_t vec_len(vec *v)
{
    if (v == NULL)
        return 0;

    return v->length * v->width;
}

size_t vec_find(vec *v, void *value)
{
    size_t i;

    if (v == NULL)
        return INVALID_INDEX;

    i = 0;

    while (i < v->length)
    {
        if (memcmp(vec_get(v, i), value, v->width) == 0)
            return i;
        ++i;
    }

    return INVALID_INDEX;
}

size_t vec_rfind(vec *v, void *value)
{
    size_t i;

    if (v == NULL)
        return INVALID_INDEX;

    i = v->length;

    while (i)
    {
        void *cmp = vec_get(v, i);

        if (cmp == NULL)
            return INVALID_INDEX;

        if (memcmp(cmp, value, v->width) == 0)
            return i;
        --i;
    }

    return INVALID_INDEX;
}

int vec_insert(vec *v, size_t i, void *value)
{
    ptrdiff_t offset;
    intptr_t  ptr;

    if (v == NULL)
        ERR(NULL_VEC, -1);

    if (i > v->length)
        ERR(INVALID_INDEX, -1);

    if (value == NULL)
        ERR(NULL_VALUE, -1);

    offset = i * v->width;
    ptr    = (intptr_t)v->data;

    v->length += v->width;

    if (vec_resize(v) == -1)
        return -1;

    memmove((void*)(ptr + offset + v->width),
            (void*)(ptr + offset), (v->length - offset));

    if (vec_set(v, i, value) == -1)
        return -1;

    return 0;
}

int vec_remove(vec *v, void *value)
{
    return vec_del(v, vec_find(v, value));
}

int vec_del(vec *v, size_t i)
{
    ptrdiff_t offset;
    intptr_t  ptr;

    if (i > vec_len(v))
        ERR(INVALID_INDEX, -1);

    offset = i * v->width;
    ptr    = (intptr_t)v->data;

    memmove((void*)(ptr + offset),
            (void*)(ptr + offset + v->width), (v->length - offset - 1));

    v->length -= v->width;

    if (vec_resize(v) == -1)
        return -1;

    return 0;
}

const char *vec_err_str()
{
    if (vecerr == E_VEC_NULL_VALUE)
        return "E_VEC_NULL_VALUE";

    if (vecerr == E_VEC_NULL_VEC)
        return "E_VEC_NULL_VEC";

    if (vecerr == E_VEC_INVALID_INDEX)
        return "E_VEC_INVALID_INDEX";

    if (vecerr == E_VEC_NO_MEMORY)
        return "E_VEC_NO_MEMORY";

    if (vecerr == E_VEC_INVALID_WIDTH)
        return "E_VEC_INVALID_WIDTH";

    return "<Unkown vec error>";
}
