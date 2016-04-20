#include <stdlib.h>

#include "io/key.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_NULL {0, ""}
#define VEC_TYPED_TYPE key
#define VEC_TYPED_NAME keys
#include "container/typed_vec.h"

#include "container/table.h"
#include "hook.h"

#include "io/keymap.h"

typedef struct keytree_s keytree;

typedef enum
{
    kmap,
    leaf
} keytree_type;

struct keytree_s
{
    keytree *parent;
    union
    {
        table *subs;
        hook h;
    } cont;
    keytree_type type;
};

struct keymap_s
{
    vec_keys *keys;
    keytree  *curr;
    keytree  *root;
    hook      unknown;
};

static int keytree_delete_leaf(keytree *tree, vec_keys *keys, size_t index);
static int keytree_delete_map(keytree *tree, vec_keys *keys, size_t index);
static int keytree_delete(keytree *tree, vec_keys *keys, size_t index);

static void keytree_create_map(keytree *ptr);
static void keytree_create_leaf(keytree *ptr);

static hook *keytree_get(keytree *tree, vec_keys *keys, size_t index);

static int keytree_add(keytree *tree, vec_keys *keys, size_t index);

static int keytree_delete_leaf(keytree *tree, vec_keys *keys, size_t index)
{
    ASSERT(vec_len(keys) == index, high, return -1);

    hook_free(tree->cont.h);
    free(tree);

    return 1;
}

static int keytree_delete_map(keytree *tree, vec_keys *keys, size_t index)
{
    int      subrtn;
    keytree *sub;
    key      k;

    ASSERT(vec_len(keys) > index, high, return -1);

    k = vec_keys_get(keys, index);

    sub = table_get(tree->cont.subs, &k);

    ASSERT(sub != NULL, high, return -1);

    TRACE((subrtn = keytree_delete(sub, keys, index + 1)) != -1,
          return -1);

    if (subrtn == 0 || index == 0)
        return 0;

    table_delete(tree->cont.subs, &k);

    if (table_len(tree->cont.subs))
        return 0;

    table_free(tree->cont.subs);
    free(tree);

    return 1;
}

static int keytree_delete(keytree *tree, vec_keys *keys, size_t index)
{
    if (tree->type == leaf)
        return keytree_delete_leaf(tree, keys, index);

    if (tree->type == kmap)
        return keytree_delete_map(tree, keys, index);

    return -1;
}

int keymap_delete(keymap *map, vec *keys)
{
    int rtn;

    rtn = keytree_delete(map->root, (vec_keys *)keys, 0);

    if (rtn == -1)
        return -1;

    return 0;
}

static void keytree_create_leaf(keytree *ptr)
{
    hook_add(h, 1);
    ptr->type   = leaf;
    ptr->cont.h = h;
}

static void keytree_create_map(keytree *ptr)
{
    ptr->type = kmap;
    ptr->cont.subs = table_init(
        sizeof(keytree), sizeof(key), NULL, NULL, NULL);
}

static keytree *keytree_init(void)
{
    keytree *rtn;

    rtn = malloc(sizeof(keytree));

    return rtn;
}

keymap *keymap_init(void)
{
    keymap  *rtn;
    keytree *root;

    hook_add(h, 1);
    rtn = malloc(sizeof(keymap));
    root = keytree_init();
    keytree_create_map(root);

    rtn->keys = vec_keys_init();
    rtn->curr = root;
    rtn->root = root;
    rtn->unknown = h;
    /* DO UNKNOWN HOOKS */

    return rtn;
}

void keymap_clear(keymap *map)
{
    vec_keys_delete(map->keys, 0, vec_len(map->keys));
    map->curr = map->root;
}

int keymap_press(keymap *map, key k)
{
    keytree *sub;
    hook     hk;

    vec_keys_insert(map->keys, vec_len(map->keys), 1, &k);

    sub = table_get(map->curr->cont.subs, &k);

    if (sub == NULL)
        hk = map->unknown;

    else if (sub->type == kmap)
    {
        map->curr = sub;
        return 0;
    }

    else if (sub->type == leaf)
        hk = sub->cont.h;

    hook_call(hk, map->keys);
    keymap_clear(map);

    return 0;
}

static hook *keytree_get(keytree *tree, vec_keys *keys, size_t index)
{
    keytree *sub;
    key      k;

    if (tree->type == leaf)
    {
        ASSERT(vec_keys_len(keys) == index, high, return NULL);

        return &(tree->cont.h);
    }

    ASSERT(vec_keys_len(keys) > index, high, return NULL);

    k   = vec_keys_get(keys, index);
    sub = table_get(tree->cont.subs, &k);

    ASSERT_PTR(sub, high, return NULL);

    return keytree_get(sub, keys, index + 1);
}

hook *keymap_get(keymap *map, vec *keys)
{
    return keytree_get(map->root, (vec_keys *)keys, 0);
}

static int keytree_add(keytree *tree, vec_keys *keys, size_t index)
{
    key k;
    keytree *sub;

    k = vec_keys_get(keys, index);

    ASSERT(tree->type == kmap, high, return -1);

    sub = table_get(tree->cont.subs, &k);

    /* Is this the last key */
    if (index == vec_keys_len(keys) - 1)
    {
        if (sub) return 0;

        sub = keytree_init();
        keytree_create_leaf(sub);

        table_set(tree->cont.subs, &k, sub);

        free(sub);

        return 0;
    }

    if (sub)
        return keytree_add(sub, keys, index + 1);

    sub = keytree_init();
    keytree_create_map(sub);

    table_set(tree->cont.subs, &k, sub);

    free(sub);

    return 0;
}

int keymap_add(keymap *map, vec *keys)
{
    return keytree_add(map->root, (vec_keys *)keys, 0);
}

hook *keymap_get_unknown_hook(keymap *k)
{
    return &(k->unknown);
}
