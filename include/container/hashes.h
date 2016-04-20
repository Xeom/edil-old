#if !defined(CONTAINER_HASHES_H)
# define CONTAINER_HASHES_H
# include "head.h"

# include "container/table.h"

hash hashes_key_str(void *k);

hash hashes_key_str_trans(void *k);

hash hashes_mem(char *mem, size_t n);

#endif /* CONTAINER_HASHES_H */
