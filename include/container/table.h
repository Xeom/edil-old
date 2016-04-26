#if !defined(CONTAINER_TABLE_H)
# define CONTAINER_TABLE_H
# include "head.h"

#include <stddef.h>
#include <stdint.h>

typedef struct table_s table;

typedef long unsigned hash;

typedef hash (*hashfunct)(void *);
typedef int  (*keqfunct)(void *, void *);

table *table_init(
    size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval);

size_t table_len(table *t);

void table_free(table *t);

int table_set(table *t, void *k, void *value);

void *table_get(table *t, void *k);

int table_delete(table *t, void *k);

#endif /* CONTAINER_TABLE_H */
