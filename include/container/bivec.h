#if !defined(CONTAINER_BIVEC_H)
# define CONTAINER_BIVEC_H
# include "head.h"

typedef struct bivec_s bivec;

typedef int  (*compfunct)(char *, char *);

bivec *bivec_init(size_t width, size_t keywidth, compfunct cmpf);

void *bivec_next(bivec *bv, void *iter);

void *bivec_get_key(bivec *bv, void *iter);

void *bivec_find(bivec *bv, char *k);

void *bivec_get(bivec *bv, void *k);

int bivec_insert(bivec *bv, char *k, char *value);

int bivec_count(bivec *bv, char *k);

int bivec_delete(bivec *bv, char *iter);

#endif /* CONTAINER_BIVEC_H */
