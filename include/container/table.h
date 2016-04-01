#if !defined(CONTAINER_TABLE_H)
# define CONTAINER_TABLE_H
# include "head.h"

#include <stddef.h>
#include <stdint.h>

typedef struct table_s table;

typedef uintptr_t key;
typedef long unsigned hash;

typedef hash (*hashfunct)(key);
typedef int  (*keqfunct)(key, key);

table *table_init(size_t width, hashfunct hshf, keqfunct keqf);

int table_set(table *t, key k, void *value);

void *table_get(table *t, key k);

int table_delete(table *t, key k);

#endif /* CONTAINER_TABLE_H */
