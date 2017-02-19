#include <string.h>
#include <stdlib.h>

#include "container/vec.h"
#include "container/hashes.h"
#include "err.h"

#include "container/table.h"

typedef struct table_item_s table_item;

#define TABLE_MAX_USAGE(cap) ((cap >> 2) + (cap >> 3))
#define TABLE_MIN_USAGE(cap) ((cap >> 3) + (cap >> 6))

#define TABLE_MIN_CAP 16

static inline char *table_index_key (table *t, size_t index);
static inline char *table_index_data(table *t, size_t index);

static int table_realloc       (table *t, size_t newcap);
static int table_resize_bigger (table *t);
static int table_resize_smaller(table *t);

static size_t table_item_size(table *t);

static void   table_key_setnull(table *t,       char *k);
static int    table_key_isnull (table *t, const char *k);
static hash   table_key_hash   (table *t, const char *k);
static int    table_key_eq     (table *t, const char *a, const char *b);

static size_t table_find_key(table *t, const void *k, int *new);

table *table_init(
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval)
{
    table *rtn;

    rtn = malloc(sizeof(table));

    ASSERT_PTR(rtn, terminal, return NULL);

    return table_create(rtn, width, keywidth, hshf, keqf, nullval);
}

table *table_create(
    void *mem,
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval)
{
    table *rtn;

    rtn = (table *)mem;

    ASSERT_PTR(rtn, high, return NULL);

    rtn->hshf     = hshf;
    rtn->keqf     = keqf;
    rtn->capacity = 0;
    rtn->usage    = 0;
    rtn->width    = width;
    rtn->keywidth = keywidth;
    rtn->nullval  = nullval;
    rtn->data     = NULL;

    TRACE_INT(table_realloc(rtn, TABLE_MIN_CAP),
              return NULL);

    return rtn;
}

void ctable_free(table *t)
{
    size_t cap, ind;

    cap = t->capacity;

    for (ind = 0; ind < cap; ++ind)
    {
        char **k, **v;

        k = (char **)table_index_key(t, ind);

        if (table_key_isnull(t, (char *)k))
            continue;

        v = (char **)table_index_data(t, ind);

        free(*k);
        free(*v);
    }

    table_free(t);
}

void table_free(table *t)
{
    if (!t) return;

    free(t->data);
    free(t);
}

void table_kill(table *t)
{
    if (!t) return;

    free(t->data);
}

size_t table_len(table *t)
{
    ASSERT_PTR(t, high, return 0);

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

static int table_key_isnull(table *t, const char *k)
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

static inline hash table_key_hash(table *t, const char *k)
{
    if (t->hshf)
        return (hash)((t->hshf)(k));
    else
        return hashes_mem(k, t->keywidth);
}

static inline int table_key_eq(table *t, const char *a, const char *b)
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
        ASSERT_PTR(t->data = realloc(t->data, table_item_size(t) * newcap),
                   terminal, return -1);

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

static size_t table_find_key(table *t, const void *k, int *new)
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

int ctable_set(table *t, const char *k, const char *value)
{
    size_t ind;

    int new;

    char *newk;
    char *newv;

    ind = table_find_key(t, &k, &new);

    if (new)
    {
        newk = malloc(strlen(k)     + 1);
        newv = malloc(strlen(value) + 1);

        strcpy(newk, k);
        strcpy(newv, value);

        table_set(t, &newk, &newv);

        return 0;
    }
    else
    {
        newv = *(char **)table_index_data(t, ind);
        newk = *(char **)table_index_key(t, ind);

        newv = realloc(newv, strlen(value) + 1);

        strcpy(newv, value);

        table_set(t, &newk, &newv);

        return 0;
    }
}

int table_set(table *t, const void *k, const void *value)
{
    int    new;
    size_t ind;

    ASSERT_PTR((char *)k, high,
               return -1);

    ind = table_find_key(t, k, &new);

    memcpy(table_index_key(t, ind), k, t->keywidth);

    if (value)
        memcpy(table_index_data(t, ind), value, t->width);

    if (new) ++(t->usage);

    TRACE_INT(table_resize_bigger(t),
              return -1);

    return 0;
}

char *ctable_get(table *t, const char *k)
{
    char **rtn;

    rtn = table_get(t, &k);

    if (!rtn)
        return NULL;

    return *rtn;
}

void *table_get(table *t, const void *k)
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

int ctable_delete(table *t, const char *k)
{
    int new, rtn;
    size_t ind;
    char *oldk;
    char *oldv;

    ind = table_find_key(t, &k, &new);

    if (new) return -1;

    oldk = *(char **)table_index_key(t, ind);
    oldv = *(char **)table_index_data(t, ind);

    rtn = table_delete(t, &k);

    free(oldk);
    free(oldv);

    return rtn;
}

int table_delete(table *t, const void *k)
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

void *table_iter_first(table *t)
{
    size_t size;
    void *item;

    if (!t->usage)
        return NULL;

    size = table_item_size(t);
    item = t->data;

    while (table_key_isnull(t, item))
        item = ADDPTR(item, size);

    return ADDPTR(item, t->keywidth);
}

void *table_iter_next(table *t, void *item)
{
    size_t size;
    void *last;

    item = SUBPTR(item, t->keywidth);

    size = table_item_size(t);
    last = t->data + size * (t->capacity - 1);

    while (item < last)
    {
        item = ADDPTR(item, size);

        if (!table_key_isnull(t, item))
            return ADDPTR(item, t->keywidth);
    }

    return NULL;
}

