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

#endif /* BUFFER_LINE_H */
