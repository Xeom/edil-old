#if !defined(CONTAINER_VEC_H)
# define CONTAINER_VEC_H
# include "head.h"

# include <stddef.h>
# include <stdint.h>

/* Used to represent an invalid index (duh) */
# define INVALID_INDEX SIZE_MAX

typedef struct vec_s vec;

struct vec_s
{
    void *data;       /* The memory contained by the vector                       */
    size_t width;     /* The width, in bytes, of each item in the vector          */
    size_t length;    /* The total length, in bytes of all items in the vector    */
    size_t capacity;  /* The amount, in bytes, of memory allocated for the vector */
};

/* Used for representing vector errors */
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

/* The last vector error */
extern vecerr_t vecerr;

/*
 * Initialize and return a new vector.
 *
 * width is the size, in bytes, of each item in the vector. e.g. sizeof(int) for a vector
 * of integers
 *
 */
vec *vec_init(size_t width);

/*
 * Initialize a vector in memory that is already allocated. The function returns the
 * argument mem, or NULL on failure.
 *
 * width is the size, in bytes of each item in the vector. e.g. sizeof(int) for a vector
 * of integers.
 *
 * mem is a pointer to memory. It must be at least sizeof(vec) bytes long.
 */
vec *vec_create(size_t width, void *mem);

/*
 * Free a vector's contents, and pointer.
 *
 * v is the vector to free.
 *
 */
void vec_free(vec *v);

/*
 * Free only a vector's contents.
 *
 * v is the vector to free..
 *
 */
void vec_kill(vec *v);

/*
 * Get a pointer to an item in the vector. The items are represented as a simple array in
 * memory, so vec_item() + vec->width will provide the next item in the vector.
 *
 * v is the vector whose items we want to access.
 *
 * index is the index(starting at 0) of the item whose pointer we want to access
 *
 */
void *vec_item(vec *v, size_t index);

/*
 * Delete items from a vector. This decreases the indices of items after those deleted.
 *
 * v is the vector to delete from.
 *
 * index is the index to start the deletion at.
 *
 * n is the amount of items to delete, starting at index.
 *
 */
int vec_delete(vec *v, size_t index, size_t n);

/*
 * Insert items into a vector from a pointer. Multiple items can be inserted, either
 * from another vector (accessed via vec_item), or from a normal C array or string.
 * The items can be inserted anywhere in the vector, including at the end and start.
 *
 * v is the vector to insert into.
 *
 * index is the index that the first new item to be inserted will have. e.g. 0 to append
 * to the start of the vector, and vec_len(v) to push to the end.
 *
 * n is the number of items to read from new, and insert into the vector.
 *
 * new is the pointer to read items to insert from.
 *
 */
int vec_insert(vec *v, size_t index, size_t n, const void *new);

/*
 * Returns the length of a vector, in number of items, not bytes.
 *
 * v is the vector whose length we want.
 *
 */
size_t vec_len(vec *v);

/*
 * Find the index of an item in a vector, starting searching at the beginning.
 * Return INVALID_INDEX on error, or if the item is not in the vector.
 *
 * v is the vector to search in.
 *
 * item is a pointer to the item to search for.
 *
 */
size_t vec_find(vec *v, const void *item);

/*
 */
int vec_contains(vec *v, const void *item);

/*
 * Find the index of an item in a vector, starting searching at the final item.
 * Return INVALID_INDEX on error, or if the item is not in the vector.
 *
 * v is the vector to search in.
 *
 * item is a pointer to the item to search for.
 *
 */
size_t vec_rfind(vec *v, const void *item);

/*
 * Return a new vector, containing a range of items from an existing vector.
 *
 * v is the vector to copy items from.
 *
 * index is the index of the first item in v to copy.
 *
 * n is the number of items to copy to the new vector.
 *
 */
vec *vec_cut(vec *v, size_t index, size_t n);

/*
 * Return the name, as a a string, of the last error that happened in a vec operation.
 *
 */
const char *vec_err_str(void);

/*
 * Perform a chunk of code, once for every item in a vector, starting at index 0,
 * dereferencing and casting each item of the vector to a specified type as a 
 * specified name.
 *
 * vec is the vector it iterate across.
 *
 * type is the type to dereference each item of the vector to.
 *
 * name is the name to assign each item
 *
 * code is the code to perform for each item of the vector.
 *
 */
#define vec_foreach(vec, type, name, code)               \
    {                                                    \
    size_t _vec_len, _vec_index, _vec_index_next;        \
    _vec_len = vec_len(vec);                             \
    _vec_index_next = 0;                                 \
    while ((_vec_index = _vec_index_next) < _vec_len)    \
        {                                                \
            type name;                                   \
            _vec_index_next = _vec_index + 1;            \
            name = *(type *)vec_item(vec, _vec_index);   \
            {code;}                                      \
        }                                                \
    }

/*
 * Perform a chunk of code, once for every item in a vector, starting at the last item,
 * dereferencing and casting each item of the vector to a specified type as a
 * specified name.
 *
 * vec is the vector it iterate across.
 *
 * type is the type to dereference each item of the vector to.
 *
 * name is the name to assign each item
 *
 * code is the code to perform for each item of the vector.
 *
 */
#define vec_rforeach(vec, type, name, code)              \
    {                                                    \
        size_t _vec_index;                               \
        _vec_index = vec_len(vec);                       \
        while (_vec_index--)                             \
        {                                                \
            type name;                                   \
            name = *(type *)vec_item(vec, _vec_index);   \
            {code;}                                      \
        }                                                \
    }                                                    \

/*
 * Macro to insert items at the end of a vector.
 *
 * vec is the vector to insert into
 *
 * n is the number of items to insert.
 *
 * item is a pointer where the items to insert are stored.
 *
 */
#define vec_insert_end(vec, n, item) vec_insert(vec, vec_len(vec), n, item)


/*
 * Macro to insert items at the start of a vector.
 *
 * vec is the vector to insert into
 *
 * n is the number of items to insert.
 *
 * item is a pointer where the items to insert are stored.
 *
 */
#define vec_insert_start(vec, n, item) vec_insert(vec, 0, n, item)

/*
 * Macro to insert the contents of one vector into another. This does not affect the vector
 * being read from.
 *
 * vec is the vector to insert into.
 *
 * index is the index to start inserting at.
 *
 * other is the vec whose contents are going to be inserted.
 *
 */
#define vec_insert_vec(vec, index, other) vec_insert(vec, index, vec_len(other), vec_item(other, 0))

#endif /* CONTAINER_VEC_H */
