#include <string.h>
#include <stdlib.h>

#include "container/vec.h"
#include "container/hashes.h"
#include "err.h"

#include "container/table.h"

typedef struct table_item_s table_item;

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

#define TABLE_MAX_USAGE(cap) ((cap >> 2) + (cap >> 3))
#define TABLE_MIN_USAGE(cap) ((cap >> 3) + (cap >> 6))

#define TABLE_MIN_CAP 16

static inline char *table_index_key (table *t, size_t index);
static inline char *table_index_data(table *t, size_t index);

static int table_realloc       (table *t, size_t newcap);
static int table_resize_bigger (table *t);
static int table_resize_smaller(table *t);

static size_t table_item_size(table *t);

static void   table_key_setnull(table *t, char *k);
static int    table_key_isnull (table *t, char *k);
static hash   table_key_hash   (table *t, char *k);
static int    table_key_eq     (table *t, char *a, char *b);

static size_t table_find_key(table *t, char *k, int *new);

table *table_init(
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval)
{
    table *rtn;

    rtn = malloc(sizeof(table));

    rtn->hshf     = hshf;
    rtn->keqf     = keqf;
    rtn->capacity = 0;
    rtn->usage    = 0;
    rtn->width    = width;
    rtn->keywidth = keywidth;
    rtn->nullval  = nullval;
    rtn->data     = NULL;

    table_realloc(rtn, TABLE_MIN_CAP);

    return rtn;
}

void table_free(table *t)
{
    free(t->data);
    free(t);
}

size_t table_len(table *t)
{
    return t->usage;
}

/* These functions get data and keys from tables at specific indexes */

static inline char *table_index_key(table *t, size_t index)
{
    return ADDPTR(t->data, table_item_size(t) * index);
}

static inline char *table_index_data(table *t, size_t index)
{
    return ADDPTR(t->data, table_item_size(t) * index + t->keywidth);
}

static inline size_t table_item_size(table *t)
{
    return t->keywidth + t->width;
}

int table_key_isnull(table *t, char *k)
{
    if (t->nullval)
        return memcmp(k, t->nullval, t->keywidth) == 0;
    else
    {/* TODO: Optimize the fuck outta this cunt */
        size_t n;
        n = t->keywidth;

        while (n--)
            if (*(k++))
                return 0;

        return 1;
    }
}

static void table_key_setnull(table *t, char *k)
{
    if (t->nullval)
        memcpy(k, t->nullval, t->keywidth);
    else
        memset(k, 0, t->keywidth);
}

static inline hash table_key_hash(table *t, char *k)
{
    if (t->hshf)
        return (hash)((t->hshf)(k));
    else
        return hashes_mem(k, t->keywidth);
}

static inline int table_key_eq(table *t, char *a, char *b)
{
    if (t->keqf)
        return (t->keqf)(a, b);
    else
        return memcmp(a, b, t->keywidth) == 0;
}

static int table_realloc(table *t, size_t newcap)
{
    size_t cap, ind;

    cap         = t->capacity;
    t->capacity = newcap;

    if (newcap > cap)
        t->data = realloc(t->data, table_item_size(t) * newcap);

    for (ind = cap; ind < newcap; ++ind)
        table_key_setnull(t, table_index_key(t, ind));

    for (ind = 0; ind < cap; ++ind)
    {
        size_t newind;
        char  *k;

        k = table_index_key(t, ind);

        if (table_key_isnull(t, k))
            continue;

        newind = table_find_key(t, k, NULL);

        if (newind == ind)
            continue;

        memcpy(table_index_key(t, newind), k, table_item_size(t));
        table_key_setnull(t, k);
    }

    if (cap > newcap)
        t->data = realloc(t->data, table_item_size(t) * newcap);

    return 0;
}

static int table_resize_bigger(table *t)
{
    if (t->usage <= TABLE_MAX_USAGE(t->capacity))
        return 0;

    table_realloc(t, t->capacity << 1);

    return 0;
}

static int table_resize_smaller(table *t)
{
    if (t->usage > TABLE_MIN_USAGE(t->capacity) ||
        t->capacity >> 1 < TABLE_MIN_CAP * t->width)
        return 0;

    table_realloc(t, t->capacity >> 1);

    return 0;
}

static size_t table_find_key(table *t, char *k, int *new)
{
    size_t ind, cap;
    char  *compk;

    cap  = t->capacity;
    ind  = table_key_hash(t, k) % cap;

    if (new) *new = 0;

    for (;;)
    {
        compk = table_index_key(t, ind);

        if (table_key_isnull(t, compk))
            break;

        if (table_key_eq(t, k, compk))
            return ind;

        ind++;
        if (ind >= cap)
            ind = 0;
    }

    if (new) *new = 1;

    return ind;
}

int table_set(table *t, void *k, void *value)
{
    int    new;
    size_t ind;

    ASSERT_PTR(value, high,
               return -1);

    ASSERT_PTR((char *)k, high,
               return -1);

    ind = table_find_key(t, k, &new);

    memcpy(table_index_key (t, ind), k, t->keywidth);
    memcpy(table_index_data(t, ind), value, t->width);

    if (new) ++(t->usage);

    TRACE_INT(table_resize_bigger(t),
              return -1);

    return 0;
}

void *table_get(table *t, void *k)
{
    int    new;
    size_t ind;

    ASSERT_PTR(t, high,
               return NULL);
    ASSERT_PTR(k, high,
               return NULL);

    ind = table_find_key(t, k, &new);

    if (new) return NULL;

    return table_index_data(t, ind);
}

int table_delete(table *t, void *k)
{
    int    new;
    char  *item, *open, *last;
    size_t ind;

    ASSERT_PTR((char *)k, high,
               return -1);

    ind = table_find_key(t, k, &new);

    if (new) return -1;

    open = table_index_key(t, ind);
    last = table_index_key(t, t->capacity - 1);
    item = open + table_item_size(t);

    for (;;item += table_item_size(t))
    {
        char  *moditem;

        if (item > last)
            item = table_index_key(t, 0);

        if (table_key_isnull(t, item))
            break;

        moditem = table_index_key(t, table_key_hash(t, item) % t->capacity);


        if ((moditem <= open    && open    < item)    ||
            (open    <  item    && item    < moditem) ||
            (item    <  moditem && moditem < open))
        {
            memcpy(open, item, table_item_size(t));
            open = item;
        }
    }

    table_key_setnull(t, open);

    (t->usage)--;
    table_resize_smaller(t);

    return 0;
}
