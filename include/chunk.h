#ifndef CHUNK_H
# define CHUNK_H

#include <stddef.h>

#include "container/vec.h"

typedef struct chunk_s chunk;

int chunk_remove_item(chunk *c, void *item);

size_t chunk_get_startindex(chunk *c);

size_t chunk_find_item_offset(chunk *c, void *item);
size_t chunk_rfind_item_offset(chunk *c, void *item);

int chunk_insert_item(chunk *c, size_t offset, void *item);

void *chunk_get_item(chunk *c, size_t i);

size_t chunk_len(chunk *c);
chunk *chunk_prev(chunk *c);
chunk *chunk_next(chunk *c);

int chunk_is_first(chunk *c);
int chunk_is_last(chunk *c);

chunk *chunk_init(void);

void chunk_free(chunk *c);

void chunk_reset_starts(chunk *c);

int chunk_resize(chunk *c);

chunk *chunk_find_containing(chunk *c, size_t n);

#endif /* CHUNK_H */
