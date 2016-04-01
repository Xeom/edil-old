#if !defined(BUFFER_LINE_H)
# define BUFFER_LINE_H
# include "head.h"

# include <stddef.h>

typedef size_t lineno;
typedef size_t colno;

typedef struct line_s line;
typedef struct chunk_s chunk;

line *buffer_line_init(void);

void buffer_line_free(line *l);

int buffer_line_set_chunk(line *l, chunk *c);

lineno buffer_line_get_lineno(line *l);

lineno buffer_line_rget_lineno(line *l);

lineno buffer_line_get_lineno_hint(line *l, lineno hint);

#endif /* BUFFER_LINE_H */
