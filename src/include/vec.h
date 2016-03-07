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

int vec_push(vec *v, const void *data);

void *vec_pop(vec *v);

void *vec_get(vec *v, size_t index);
int   vec_set(vec *v, size_t index, const void *data);

int vec_trim(vec *v, size_t amount);

vec *vec_slice(vec *v, size_t start, size_t end);

int vec_append(vec *v, vec *other);

size_t vec_len(vec *v);

size_t vec_find(vec *v,  const void *value);
size_t vec_rfind(vec *v, const void *value);

int vec_insert(vec *v, size_t i, const void *value);
int vec_del(vec *v, size_t i);

int vec_remove(vec *v, const void *value);

const char *vec_err_str(void);

#endif /* VEC_H */
