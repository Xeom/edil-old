#if !defined(CONTAINER_HASHES_H)
# define CONTAINER_HASHES_H
# include "head.h"

# include "container/table.h"

int hashes_eq_str(const void *a, const void *b);

hash hashes_key_str(const void *k);

hash hashes_key_str_trans(const void *k);

hash hashes_mem(const char *mem, size_t n);

#endif /* CONTAINER_HASHES_H */
