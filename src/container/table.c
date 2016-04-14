#include <string.h>
#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "container/table.h"

typedef struct table_item_s table_item;

struct table_s
{
    hashfunct   hshf;
    keqfunct    keqf;
    size_t      capacity;
    size_t      usage;
    size_t      width;
    size_t      keywidth;
    char       *nullval;
    char       *data;
};

#define TABLE_MAX_USAGE(cap) ((cap >> 2) + (cap >> 3))
#define TABLE_MIN_USAGE(cap) ((cap >> 3) + (cap >> 6))

#define TABLE_MIN_CAP 16

static char *table_index_key (table *t, size_t index);
static char *table_index_data(table *t, size_t index);

static int table_realloc       (table *t, size_t newcap);
static int table_resize_bigger (table *t);
static int table_resize_smaller(table *t);

static size_t table_item_size(table *t);

static void   table_key_setnull(table *t, char *k);
static int    table_key_isnull (table *t, char *k);
static hash   table_key_hash   (table *t, char *k);
static int    table_key_eq     (table *t, char *a, char *b);

static size_t table_find_key_occ(table *t, char *k);
static size_t table_find_key    (table *t, char *k);

table *table_init(size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval)
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

static inline char *table_index_key(table *t, size_t index)
{
    return ADDPTR(t->data, table_item_size(t) * index);
}

static inline char *table_index_data(table *t, size_t index)
{
    return ADDPTR(t->data, table_item_size(t) * index + t->width);
}

static inline int table_key_isnull(table *t, char *k)
{
    return memcmp(k, t->nullval, t->keywidth) == 0;
}

static void table_key_setnull(table *t, char *k)
{
    memcpy(k, t->nullval, t->keywidth);
}

static inline size_t table_item_size(table *t)
{
    return t->keywidth + t->width;
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

        newind = table_find_key(t, k);

        if (newind == ind)
            continue;

        memcpy(k, table_index_key(t, newind), table_item_size(t));
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

static inline hash table_key_hash(table *t, char *k)
{
    if (t->hshf)
        return (hash)((t->hshf)(k));
    else
        return (hash)k;
}

static inline int table_key_eq(table *t, char *a, char *b)
{
    if (t->keqf)
        return (t->keqf)(a, b);
    else
        return (a == b);
}

static size_t table_find_key_occ(table *t, char *k)
{
    size_t ind, cap;
    char  *compk;

    cap = t->capacity;
    ind = table_key_hash(t, k) % cap;

    for (compk = table_index_key(t, ind);
         !table_key_isnull(t, compk);
         compk += table_item_size(t))
    {
        if (ind >= cap)
            ind = 0;

        if (table_key_eq(t, k, compk))
            return ind;
    }

    return INVALID_INDEX;
}

static size_t table_find_key(table *t, char *k)
{
    size_t ind, cap;
    char  *compk;

    cap = t->capacity;
    ind = table_key_hash(t, k) % cap;

    for (compk = table_index_key(t, ind);
         !table_key_isnull(t, compk);
         compk += table_item_size(t))
    {
        if (ind >= cap)
            ind = 0;

        if (table_key_eq(t, k, compk))
            return ind;
    }

    return ind;
}

int table_set(table *t, char *k, char *value)
{
    size_t ind;

    ASSERT_PTR(value, high,
               return -1);

    ASSERT_PTR((char *)k, high,
               return -1);

    ind  = table_find_key(t, k);

    memcpy(table_index_key (t, ind), k, t->keywidth);
    memcpy(table_index_data(t, ind), k, t->width);

    ++(t->usage);

    TRACE_INT(table_resize_bigger(t),
              return -1);

    return 0;
}

char *table_get(table *t, char *k)
{
    size_t ind;

    ASSERT_PTR((char *)k, high,
               return NULL);

    ind = table_find_key_occ(t, k);

    if (ind == INVALID_INDEX)
        return NULL;

    return table_index_data(t, ind);
}

int table_delete(table *t, char *k)
{
    char  *item, *previtem;
    size_t ind, modhsh;

    ASSERT_PTR((char *)k, high,
               return -1);

    modhsh   = table_key_hash(t, k) % t->capacity;
    ind      = table_find_key_occ(t, k);

    if (ind == INVALID_INDEX)
        return -1;

    previtem = table_index_key(t, ind);

    for (item = previtem + table_item_size(t);
         !table_key_isnull(t, item);
         item += table_item_size(t))
        if (table_key_hash(t, item) % t->capacity
            == modhsh)
        {
            memcpy(previtem, item, table_item_size(t));
            previtem = item;
        }

    table_key_setnull(t, previtem);

    (t->usage)--;

    table_resize_smaller(t);

    return 0;
}
