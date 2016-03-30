#ifndef CONTAINER_TABLE_H
# define CONTAINER_TABLE_H

typedef struct table_s table;

typedef uintptr_t key;
typedef long uint hash;

typedef hash (*hashfunct)(void *);
typedef int  (*keyeqfunct)(void *, void *);

int table_init(size_t width, hashfunct hshf, keqfunct keqf);

int table_set(table *t, key k, void *value);

void *table_get(table *t, key k);

int table_delete(table *t, key k);

#endif /* CONTAINER_TABLE_H */
