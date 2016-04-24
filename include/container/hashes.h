#if !defined(CONTAINER_HASHES_H)
# define CONTAINER_HASHES_H
# include "head.h"

# include "container/table.h"

int hashes_eq_str(void *a, void *b);

hash hashes_key_str(void *k);

hash hashes_key_str_trans(void *k);

hash hashes_mem_long(char *mem, size_t n);

hash hashes_mem(char *mem, size_t n);

#endif /* CONTAINER_HASHES_H */
