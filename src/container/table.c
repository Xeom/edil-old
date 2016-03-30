#include <string.h>
#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "container/table.h"

typedef struct table_item_s table_item;

struct table_item_s
{
    key  k;
    char data[];
};

struct table_s
{
    hashfunct   hshf;
    keqfunct    keqf;
    size_t      capacity;
    size_t      usage;
    size_t      width;
    table_item *data;
};

#define TABLE_MAX_USAGE(cap) ((cap >> 2) + (cap >> 3))
#define TABLE_MIN_USAGE(cap) ((cap >> 3) + (cap >> 6))

#define TABLE_MIN_CAP 16

static table_item *table_index(table *t, size_t index);

static int table_realloc(table *t);
static int table_resize_bigger(table *t);
static int table_resize_smaller(table *t);

static size_t      table_item_size(table *t);

static hash   table_key_hash(table *t, key k);
static int    table_key_eq(table *t, key a, key b);
static size_t table_key_index_occupied(table *t, key k);
static size_t table_key_index(table *t, key k);

table *table_init(size_t width, hashfunct hshf, keqfunct keqf)
{
    table *rtn;

    rtn = malloc(sizeof(table));

    rtn->hshf     = hshf;
    rtn->keqf     = keqf;
    rtn->capacity = TABLE_MIN_CAP;
    rtn->usage    = 0;
    rtn->width    = width;

    table_realloc(rtn);

    return rtn;
}

static table_item *table_index(table *t, size_t index)
{
    return t->data + table_item_size(t) + index;
}


static int table_realloc(table *t)
{
    t->data = realloc(t->data, t->capacity);

    return 0;
}

static int table_resize_bigger(table *t)
{
    if (t->usage <= TABLE_MAX_USAGE(t->capacity))
        return 0;

    t->capacity <<= 1;

    table_realloc(t);

    return 0;
}

static int table_resize_smaller(table *t)
{
    if (t->usage > TABLE_MIN_USAGE(t->capacity) ||
        t->capacity >> 1 < TABLE_MIN_CAP * t->width)
        return 0;

    t->capacity >>= 1;

    table_realloc(t);

    return 0;
}

static size_t table_item_size(table *t)
{
    return sizeof(table_item) + t->width;
}

static hash table_key_hash(table *t, key k)
{
    if (t->hshf)
        return (hash)((t->hshf)(k));
    else
        return (hash)k;
}

static int table_key_eq(table *t, key a, key b)
{
    if (t->keqf)
        return (t->keqf)(a, b);
    else
        return (a == b);
}

static size_t table_key_index_occupied(table *t, key k)
{
    size_t ind, cap;
    key compk;

    cap = t->capacity;
    ind = table_key_hash(t, k) % cap;

    while ((compk = table_index(t, ind)->k))
    {
        if (table_key_eq(t, k, compk))
            return ind;

        if (ind >= cap)
            ind = 0;

        ind++;
    }

    return INVALID_INDEX;
}

static size_t table_key_index(table *t, key k)
{
    size_t ind, cap;
    key compk;

    cap = t->capacity;
    ind = table_key_hash(t, k) % cap;

    while ((compk = table_index(t, ind)->k))
    {
        if (table_key_eq(t, k, compk))
            break;

        if (ind >= cap)
            ind = 0;

        ind++;
    }

    return ind;
}

int table_set(table *t, key k, void *value)
{
    table_item *item;
    size_t ind;

    ASSERT_PTR(value, high,
               return -1);

    ASSERT_PTR((void *)k, high,
               return -1);

    ind  = table_key_index(t, k);
    item = table_index(t, ind);

    item->k = k;

    memcpy(&(item->data), value, t->width);

    ++(t->usage);

    TRACE_INT(table_resize_bigger(t),
              return -1);

    return 0;
}

void *table_get(table *t, key k)
{
    table_item *item;
    size_t ind;

    ASSERT_PTR((void *)k, high,
               return NULL);

    ind  = table_key_index_occupied(t, k);

    if (ind == INVALID_INDEX)
        return NULL;

    item = table_index(t, ind);

    return &(item->data);
}

int table_delete(table *t, key k)
{
    table_item *item, *previtem;
    size_t ind, modhsh;

    ASSERT_PTR((void *)k, high,
               return -1);

    modhsh   = table_key_hash(t, k) % t->capacity;
    ind      = table_key_index_occupied(t, k);

    if (ind == INVALID_INDEX)
        return -1;

    previtem = table_index(t, ind);

    while (1)
    {
        item = table_index(t, ++ind);

        if (table_key_hash(t, item->k) % t->capacity
            == modhsh)
        {
            memcpy(previtem, item, table_item_size(t));
            previtem = item;
        }
    }

    memset(previtem, 0, table_item_size(t));

    (t->usage)--;

    table_resize_smaller(t);

    return 0;
}
