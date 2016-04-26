#include <string.h>
#include <alloca.h>

#include "container/vec.h"

#include "container/bivec.h"

struct bivec_s
{
    vec       data;
    compfunct cmpf;
    size_t    width;
    size_t    keywidth;
};

static inline size_t bivec_item_size(bivec *bv);

bivec *bivec_init(size_t width, size_t keywidth, compfunct cmpf);

void *bivec_next(bivec *bv, void *iter)
{
    void *last;

    last = vec_item((vec *)bv, vec_len((vec *)bv));

    iter = ADDPTR(iter, bivec_item_size(bv));

    if (iter > last)
        return NULL;

    return iter;
}

void *bivec_get_key(bivec *bv, void *iter)
{
    iter = ADDPTR(iter, bv->width);

    return iter;
}

static inline size_t bivec_item_size(bivec *bv)
{
    return bv->width + bv->keywidth;
}

static size_t bivec_index(bivec *bv, char *k)
{
    size_t index, range;

    index = 0;
    range = vec_len((vec *)bv);

    while (range > 1)
    {
        void *pivot;

        pivot = vec_item((vec *)bv, index + range / 2);

        if (bv->cmpf(ADDPTR(pivot, bv->width), k) == -1)
        {
            index += range / 2;
            range -= range / 2;
        }
        else
            range /= 2;
    }

    index++;

    return index;
}

/* Return the first item equal or greater than k */
void *bivec_find(bivec *bv, char *k)
{
    size_t index;

    index = bivec_index(bv, k);

    if (index >= vec_len((vec *)bv))
        return NULL;

    return vec_item((vec *)bv, index);
}

void *bivec_get(bivec *bv, void *k)
{
    void  *item;
    size_t index;

    index = bivec_index(bv, k);

    if (index >= vec_len((vec *)bv))
        return NULL;

    item = vec_item((vec *)bv, index);

    if (bv->cmpf(ADDPTR(item, bv->width), k) != 0)
        return NULL;

    return item;
}

/* Insert a value */
int bivec_insert(bivec *bv, char *k, char *value)
{
    size_t index;
    void  *new;

    index = bivec_index(bv, k);
    new   = alloca(bivec_item_size(bv));

    memcpy(value, new, bv->width);
    memcpy(k, ADDPTR(new, bv->width), bv->keywidth);

    vec_insert((vec *)bv, index, 1, new);

    return 0;
}

/* Count the number of items in a bivec with a key 
int bivec_count(bivec *bv, char *k)
{
}
*/
/* Delete an iterator 
int bivec_delete(bivec *bv, char *iter)
{
}
*/
