#ifndef VEC_H
#define VEC_H
#include <stddef.h>
#include <stdint.h>

#define INVALID_INDEX SIZE_MAX

struct vec_s
{
    void *data;
    size_t width;
    size_t length;
    size_t capacity;
};

typedef enum
{
    E_VEC_OK,
    E_VEC_NULL_VALUE,
    E_VEC_NULL_VEC,
    E_VEC_INVALID_INDEX,
    E_VEC_INVALID_VALUE,
    E_VEC_NO_MEMORY,
    E_VEC_INVALID_WIDTH
} vecerr_t;

extern vecerr_t vecerr;

typedef struct vec_s vec;

vec *vec_init(size_t width);

void vec_free(vec *v);

void *vec_item(vec *v, size_t index);

int vec_delete(vec *v, size_t index, size_t n);

int vec_insert(vec *v, size_t index, size_t n, const void *new);

size_t vec_len(vec *v);

size_t vec_find(vec *v, const void *item);

size_t vec_rfind(vec *v, const void *item);

const char *vec_err_str(void);

#endif /* VEC_H */
