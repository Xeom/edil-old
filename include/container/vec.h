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
    void *data;      /* The memory contained by the vector.               */
    size_t width;    /* The width, in bytes, of each item in the vector.  */
    size_t length;   /* The length, in bytes, of all items in the vector. */
    size_t capacity; /* The number of bytes, allocated as data.           */
};

/*
 * Initialize and return a pointer to a new, empty, vector with items of a
 * specified width.
 *
 * @param width The size, in bytes, of every item in the vector.
 *
 * @return      A pointer to the new vector, NULL on failure.
 *
 */
vec *vec_init(size_t width);

/*
 * Initialize a new, empty, vector, with items of a specified width, in memory
 * that is already allocated.
 *
 * @param mem   A pointer to the memory to initialize the vector in. Should be
 *              as large as sizeof(vec).
 * @param width The size, in bytes, of every item in the vector.
 *
 * @return      mem on success, NULL on failure.
 *
 */
vec *vec_create(void *mem, size_t width);

/*
 * Free the contents of a vector, and the vector itself.
 *
 * @param v A pointer to the vector to free.
 *
 */
void vec_free(vec *v);

/*
 * Free only the contents of a vector, leaving its pointer unaffected. Intended
 * to reverse vec_create.
 *
 * @param v A pointer to the vector to kill.
 *
 */
void vec_kill(vec *v);

/*
 * Get a pointer to an item in a vector. The items are represented as a simple
 * array, like you'd expect, in memory, so offsets can be added or subtracted
 * to or from the results of this function to obtain other indices.
 *
 * @param v     A pointer to the vector to access items in.
 * @param index The index (vecs are 0-indexed) to fetch.
 *
 * @return      A pointer to the requested item in the vector, NULL on failure.
 *
 */
void *vec_item(vec *v, size_t index);


/*
 * Delete items from a vector. Note that this shifts the indices of items after
 * those deleted backwards, so pointers to them will break.
 *
 * @param v     A pointer to the vector to delete items from.
 * @param index The index of the first item to delete.
 * @param n     The number of items to delete.
 *
 */
int vec_delete(vec *v, size_t index, size_t n);

/*
 * Insert items into a vector from a pointer. Multiple items are taken from
 * the poinnter in sequence, so ensure that there is safe memory there. The 
 * index supplied will become the index of the first item inserted.
 *
 * @param v     A pointer to the vector to insert items into.
 * @param index The index where the first newly inserted item should go.
 * @param n     The number of items to insert.
 * @param new   A pointer to the data to insert.
 *
 * @return      0 on success, -1 on error.
 *
 */
int vec_insert(vec *v, size_t index, size_t n, const void *new);

/*
 * Returns the number of items currently in a vector.
 *
 * @param v A pointer to a vector to measure the length of.
 *
 * @return  The number of items in the vector.
 *
 */
size_t vec_len(vec *v);

/*
 * Find the index of an item in a vector, starting searching at the beginning.
 *
 * @param v    A pointer to the vector to search in.
 * @param item A pointer to the item to search for.
 *
 * @return     The index of the item in the vector, INVALID_INDEX on error.
 */
size_t vec_find(vec *v, const void *item);

/*
 * Find the index of an item in a vector, starting searching at the end.
 *
 * @param v    A pointer to the vector to search in.
 * @param item A pointer to the item to search for.
 *
 * @return     The index of the item in the vector, INVALID_INDEX on error.
 */
size_t vec_rfind(vec *v, const void *item);

/*
 * Check whether a vector contains an item.
 *
 * @param v    A pointer to the vector to check for the item.
 * @param item A pointer to the item to check for.
 *
 * @return     0 if the vec doesn't contain the item, 1 if it does, -1 on error
 *
 */
int vec_contains(vec *v, const void *item);

/*
 * Binary search a vector whose items are sorted in ascending order for the
 * index of the first item which is equal or greater than the supplied item.
 * Items are compared byte-by-byte in an order determined by whether the system
 * is big or little endian. The bits that are compared are selected by means of
 * a bit mask. If all values in the vector are less than the supplied value,
 * the length of the vector is returned.
 *
 * @param v    A pointer to the vector to search.
 * @param item A pointer to the item to find.
 * @param map  A bitmask to decide which bits are relevant to the search.
 *
 * @return     The appropriate index, INVALID_INDEX on failure.
 *
 */
size_t vec_bisearch(vec *v, const void *item, const void *map);

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
#define vec_foreach(vec, type, name, code)                  \
    {                                                       \
        size_t _vec_len, _vec_index, _vec_index_next;       \
        _vec_len = vec_len(vec);                            \
        _vec_index_next = 0;                                \
        while ((_vec_index = _vec_index_next) < _vec_len)   \
        {                                                   \
            type name;                                      \
            _vec_index_next = _vec_index + 1;               \
            name = *(type *)vec_item(vec, _vec_index);      \
            {code;}                                         \
        }                                                   \
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
