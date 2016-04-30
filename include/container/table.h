#if !defined(CONTAINER_TABLE_H)
# define CONTAINER_TABLE_H
# include "head.h"

#include <stddef.h>
#include <stdint.h>

typedef long long unsigned hash;

typedef hash (*hashfunct)(void *);
typedef int  (*keqfunct)(void *, void *);

typedef struct table_s table;

struct table_s
{
    /* Both these function pointers can be null for default behavior          */

    hashfunct   hshf;     /* Function for hashing keys. Takes a pointer to the
                             first byte of a key.                             */

    keqfunct    keqf;     /* Function for testing key equality. Takes a
                             pointer to the first byte of two keys, returns 1
                             if they are equal.                               */

    size_t      capacity; /* Stores the total amount of space allocated for
                             data in num of items.                            */

    size_t      usage;    /* Stores the total amount of items stored in the
                             table.                                           */


    size_t      width;    /* Stores the size, in bytes, of the data for each 
                             item.                                            */

    size_t      keywidth; /* Stores the size, in bytes, of the key for each
                             item.                                            */

    char       *nullval;  /* Stores a block of memory representing a null key
                             value. Or NULL for default behavior.             */

    char       *data;     /* All the data for the table, stored like
                             [[key|data], [key|data], ...]                    */
};

/*
 * Initialize and return a pointer to a new table.
 *
 * Tested in unit_table.test_init
 *
 * @param width    The size, in bytes, of each value stored in the table. A
 *                 pointer to a valid block of memory of this size should be
 *                 returned by table_get and should be passed to table_set as
 *                 the value argument.
 * @param keywidth The size, in bytes, of each key stored in the table. A
 *                 pointer to a valid block of memory of this size should be
 *                 passed as the k parameter to table_ set, get and delete.
 * @param hshf     A function pointer to a hashfunction for the table. The
 *                 hashfunction takes one argument - A pointer to the key that
 *                 needs hashing, and should return a hash value. If NULL, the
 *                 table uses hashes_mem to hash keys by their value. Note that
 *                 this will not work if keys are pointers for example.
 * @param keqf     A function pointer to a key equaity function for the table.
 *                 the function should take two arguments - Both pointers to
 *                 keys, and return the integer 1 if they are equal, and 0
 *                 otherwise. If NULL, the table will simply compare the raw
 *                 values of the keys. If the keys are pointers, this will not
 *                 work.
 * @param nullval  A value representing an invalid key. This value must not be
 *                 a valid key value, since using it as a key will not work.
 *                 If NULL, the table will use a string of '\0's of appropriate
 *                 length as the null value.
 *
 * @return         A pointer to the new table, or NULL on failure.
 *
 */
table *table_init(
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval);

/*
 * Use already allocated memory to create a table.
 *
 * Tested in unit_table.test_init
 *
 * @param mem      A pointer to a block of memory to create the table in. It
 *                 must be a valid pointer to a block of at least sizeof(table)
 *                 bytes large.
 * @param width    The size, in bytes, of each value stored in the table. A
 *                 pointer to a valid block of memory of this size should be
 *                 returned by table_get and should be passed to table_set as
 *                 the value argument.
 * @param keywidth The size, in bytes, of each key stored in the table. A
 *                 pointer to a valid block of memory of this size should be
 *                 passed as the k parameter to table_ set, get and delete.
 * @param hshf     A function pointer to a hashfunction for the table. The
 *                 hashfunction takes one argument - A pointer to the key that
 *                 needs hashing, and should return a hash value. If NULL, the
 *                 table uses hashes_mem to hash keys by their value. Note that
 *                 this will not work if keys are pointers for example.
 * @param keqf     A function pointer to a key equaity function for the table.
 *                 the function should take two arguments - Both pointers to
 *                 keys, and return the integer 1 if they are equal, and 0
 *                 otherwise. If NULL, the table will simply compare the raw
 *                 values of the keys. If the keys are pointers, this will not
 *                 work.
 * @param nullval  A value representing an invalid key. This value must not be
 *                 a valid key value, since using it as a key will not work.
 *                 If NULL, the table will use a string of '\0's of appropriate
 *                 length as the null value.
 *
 * @return         Either the argument mem cast to a table pointer, or NULL on
 *                 error.
 *
 */
table *table_create(
    void *mem,
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval);

/*
 * Get the number of items in a table.
 *
 * Tested in unit_table.test_del
 *
 * @param t A pointer to the table to get the length of.
 *
 * @return  The number of items in the table.
 *
 */
size_t table_len(table *t);

/*
 * Free the data associated with a table, and the table itself. This does not
 * free the nullval passed to the table during its creation. This should be
 * free'd by the caller of table_create/table_init. This function will free the
 * memory passed to table_create if the table was initialized this way.
 *
 * Testable by valgrind in unit_table.test_del, test_init, test_set
 *
 * @param t A pointer to the table to free.
 *
 */
void table_free(table *t);

/*
 * Set the value in a table associated with a key. If a value is already
 * associated with the key, overwrite it. If it does not, create it.
 *
 * Tested by unit_table.test_set
 *
 * @param t     A pointer to the table to set a value in.
 * @param k     A pointer to the key to associate the value with. It must point
 *              to a valid block of memory with size of at least the keywidth of
 *              the table.
 * @param value A pointer to the value to associate with the key. It must point
 *              to a valid block of memory with size of at least the width of
 *              the table.
 *
 * @return      0 on success, -1 on error.
 *
 */
int table_set(table *t, void *k, void *value);

/*
 * Get the value in a table associated with a key. If there is no value
 * associated with the key, return NULL. If non-null, the value is a pointer
 * to a block of memory of size of at least the width of the table. The memory
 * is in-place in the table - it is not copied before return. So it does not
 * need to be free'd, and changing its value will affect the table.
 *
 * Tested by unit_table.test_set, test_del
 *
 * @param t A pointer to the table to get a value from.
 * @param k A pointer to the key to get the value associated with. It must point
 *          to a valid block of memory with size of at least the keywidth of the
 *          table.
 *
 * @return  A pointer to the value associated with the key. NULL on error or if
 *          there is no value associated with the key.
 *
 */
void *table_get(table *t, void *k);

/*
 * Delete a value associated with a key from a table.
 *
 * Tested by unit_table.test_del
 *
 * @param t A pointer to the table to delete a value from.
 * @param k A pointer to the key to delete the value associated with. It must
 *          point to a valid block of memory with size of at least the keywidth
 *          of the table.
 *
 * @return  0 on success, -1 on error.
 *
 */
int table_delete(table *t, void *k);

#endif /* CONTAINER_TABLE_H */
