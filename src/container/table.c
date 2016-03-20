struct table_item_s
{
    hash  hash;
    void *key;
    void *data;
}

struct table_s
{
    hashfunct   hfunct;
    keyeqfunct  keqfunct;
    size_t      capacity;
    size_t      usage;
    table_item *data;
};

#define TABLE_OH_FUCK_IT_DEPTH 10

typedef long unsigned int hash;
typedef hash (hashfunct)(void *)
typedef int  (keyeqfunct)(key *, key *)

table *table_init(hashfunct hf, keyeqfunct eqf)
{
    
}

static hash table_get_hash(table *t, void *h, unsigned int n)
{
    if (t->hfunct == NULL)
        return (intptr_t)h;
    else
        return t->hfunct(h);

    return hsh;
}

static int table_item_key_eq(table *t, table_item a, void *key)
{
    if (t->keqfunct == NULL)
        return a.key == key;
    else
        return t->keqfunct(a.key, key);
}

static table_item table_get_item_index_used_only(table *t, void *key)
{
    unsigned int n;
/* TODO: Add limiting of n */
    n   = 0;
    hsh = table_get_hash(t, key);
    do
    {
        ind = (hsh + n++) % t->capacity;
        compkey = table->data[ind]->key;

        if (!compkey)
            return INVALID_INDEX;
    } while (!table_item_key_eq(key, compkey)));

    return ind;
}

static table_item table_get_item_index(table *t, void *key)
{
    unsigned int n;

    n   = 0;
    hsh = table_get_hash(t, key);
    do
    {
        ind = (hsh + n++) % t->capacity;
        compkey = table->data[ind]->key;
    } while (compkey && !table_item_key_eq(key, compkey));

    return ind;
}

int table_insert(table *t, void *key, void *value)
{
    hash hsh;
    table_item new;

    t->usage++;

    ind = table_get_item_index(t, key);

    new.hash = hsh;
    new.key  = key;
    t->data[ind] = new;
}

int table_delete(table *t, void *key)
{
    size_t ind, n, prevn;
    table_item curr, previtem;

    ind = table_get_item_index_used_only(t, key);

    if (ind == INVALID_INDEX)
        return -1;

    n     = 0;
    prevn = 0;

    while (t->data[ind + ++n].key)
        if (t->data[ind + n].hash % t->capacity == ind)
        {
            memcpy(t->data + ind + prevn, t->data + ind + n, sizeof(table_item));
            prevn = n;
        }

    memset(t->data + prevn, 0, sizeof(table_item));
}

void *table_get(table *t, void *h)
{
}
