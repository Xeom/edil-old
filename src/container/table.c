#include "container/table.h"
#include "container/vec.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
typedef struct table_item_s table_item;

struct table_item_s
{
    size_t *key;
    void   *data;
};

struct table_s
{
    hashfunct   hfunct;
    keyeqfunct  keqfunct;
    size_t      capacity;
    size_t      usage;
    table_item *data;
};

#define TABLE_OH_FUCK_IT_DEPTH 10
#define TABLE_MIN_SIZE 32

static int  table_resize_to(table *t, size_t newcap);
static int  table_resize_bigger(table *t);
static int  table_resize_smaller(table *t);

static int  table_item_key_eq(table *t, table_item a, void *key);
static hash table_get_hash(table *t, void *h);

static size_t table_get_key_index          (table *t, void *key);
static size_t table_get_key_index_used_only(table *t, void *key);

table *table_init(hashfunct hf, keyeqfunct eqf)
{
    table *rtn;

    rtn = malloc(sizeof(table));

    rtn->hfunct = hf;
    rtn->keqfunct = eqf;
    rtn->capacity = TABLE_MIN_SIZE;
    rtn->usage = 0;
    rtn->data  = malloc(TABLE_MIN_SIZE * sizeof(table_item));
    memset(rtn->data, 0, TABLE_MIN_SIZE * sizeof(table_item));
    return rtn;
}

static int table_resize_to(table *t, size_t newcap)
{
    size_t capacity, currind, newind;
    table_item curr;

    capacity = t->capacity;
    t->capacity = newcap;
    currind  = newcap;

    while (currind < capacity)
    {
        curr = t->data[currind];

        if (curr.key == NULL)
            continue;

        newind = table_get_key_index(t, curr.key);
        memcpy(t->data + newind, t->data + currind, sizeof(table_item));

        currind++;
    }

    t->data = realloc(t->data, sizeof(table_item) * newcap);

    if (newcap > capacity)
        memset(t->data + capacity, 0, sizeof(table_item) * (newcap - capacity));

    return 0;
}

static int table_resize_bigger(table *t)
{
    if (t->usage < (t->capacity >> 2) + (t->capacity >> 3))
        return 0;

    table_resize_to(t, t->capacity << 1);

    return 0;
}

static int table_resize_smaller(table *t)
{
    if (t->usage > (t->capacity >> 3) + (t->capacity >> 6))
        return 0;

    if (t->usage >> 1 > TABLE_MIN_SIZE)
        return 0;

    table_resize_to(t, t->capacity >> 1);

    return 0;
}

/* TODO: Everything */
static hash table_get_hash(table *t, void *h)
{
    if (t->hfunct == NULL)
        return (intptr_t)h;
    else
        return t->hfunct(h);
}

static int table_item_key_eq(table *t, table_item a, void *key)
{
    if (t->keqfunct == NULL)
        return a.key == key;
    else
        return t->keqfunct(a.key, key);
}

static size_t table_get_key_index_used_only(table *t, void *key)
{
    hash hsh;
    size_t ind;
    unsigned int n;
/* TODO: Add limiting of n */
    hsh = table_get_hash(t, key);
    n   = 0;
    do
    {
        ind = (hsh + n++) % t->capacity;

        if (t->data[ind].key == NULL)
            return INVALID_INDEX;
    }
    while (!table_item_key_eq(t, t->data[ind], key));

    return ind;
}

static size_t table_get_key_index(table *t, void *key)
{
    hash hsh;
    unsigned int n;
    size_t ind;

    hsh = table_get_hash(t, key);

    n = 0;
    do
        ind = (hsh + n++) % t->capacity;
    while (t->data[ind].key &&
             !table_item_key_eq(t, t->data[ind], key));

    return ind;
}

int table_insert(table *t, void *key, void *value)
{
    size_t ind;
    table_item new;

    t->usage++;

    ind = table_get_key_index(t, key);

    if (t->data[ind].key == NULL)
        (t->usage)++;

    new.data = value;
    new.key  = key;
    t->data[ind] = new;

    table_resize_bigger(t);

    return 0;
}

int table_delete(table *t, void *key)
{
    size_t ind, n, prevn;

    ind = table_get_key_index_used_only(t, key);

    if (ind == INVALID_INDEX)
        return -1;

    (t->usage)--;
    n     = 0;
    prevn = 0;

    while (t->data[ind + ++n].key)
        if (table_get_hash(t, t->data[ind + n].key) % t->capacity == ind)
        {
            memcpy(t->data + ind + prevn, t->data + ind + n, sizeof(table_item));
            prevn = n;
        }

    memset(t->data + prevn, 0, sizeof(table_item));

    table_resize_smaller(t);

    return 0;
}

void *table_get(table *t, void *key)
{
    table_item item;
    size_t ind;

    ind = table_get_key_index_used_only(t, key);

    if (ind == INVALID_INDEX)
        return NULL;

    item = t->data[ind];

    return item.data;
}
