#include <stddef.h>

#include "container/vec.h"
#include "hook.h"

typedef struct deferline_s deferline;

extern hook buffer_deferline_on_draw;

int buffer_deferline_initsys(void);

deferline *buffer_deferline_init(vec *v);

void buffer_deferline_free(deferline *dl);

int buffer_deferline_insert(deferline *dl, size_t index, char *str);

int buffer_deferline_dump(deferline *dl);
