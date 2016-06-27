#if !defined(BUFFER_DEFERLINE_H)
# define BUFFER_DEFERLINE_H

# include <stddef.h>

# include "container/vec.h"
# include "hook.h"

typedef struct deferline_s deferline;

extern hook buffer_deferline_on_draw;

int buffer_deferline_initsys(void);

deferline *buffer_deferline_init(vec *v);

vec *buffer_deferline_get_vec(deferline *dl);

void buffer_deferline_free(deferline *dl);

int buffer_deferline_insert(deferline *dl, size_t index, const char *str);

int buffer_deferline_delete(deferline *dl, size_t index);

int buffer_deferline_dump(deferline *dl);

#endif /* BUFFER_DEFERLINE_H */
