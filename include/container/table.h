#if !defined(CONTAINER_TABLE_H)
# define CONTAINER_TABLE_H
# include "head.h"

#include <stddef.h>
#include <stdint.h>

typedef struct table_s table;

typedef long unsigned hash;

typedef hash (*hashfunct)(char *);
typedef int  (*keqfunct)(char *, char *);

table *table_init(size_t width, size_t keywidth, hashfunct hshf, keqfunct keqf, char *nullval);

int table_set(table *t, char *k, char *value);

char *table_get(table *t, char *k);

int table_delete(table *t, char *k);

#endif /* CONTAINER_TABLE_H */
