#ifndef BUFFER_CORE_H
# define BUFFER_CORE_H
# include "head.h"

typedef struct buffer_s buffer;

buffer *buffer_init(void);

void buffer_free(buffer *b);

line *buffer_insert(buffer *b, lineno ln);

int buffer_delete(buffer *b, lineno index);

line *buffer_get_line(buffer *b, lineno ln);

#endif /* BUFFER_CORE_H */
