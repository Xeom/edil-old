#if !defined(BUFFER_LINE_H)
# define BUFFER_LINE_H
# include "head.h"

# include <stddef.h>
# include "container/vec.h"

typedef size_t lineno;
typedef size_t colno;

typedef struct line_s line;
typedef struct chunk_s chunk;

line *buffer_line_init(void);

void buffer_line_free(line *l);

vec *buffer_line_get_vec(line *l);

int buffer_line_set_vec(line *l, vec *v);

#endif /* BUFFER_LINE_H */
