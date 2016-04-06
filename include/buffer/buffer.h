#if !defined(BUFFER_CORE_H)
# define BUFFER_CORE_H
# include "head.h"

# include "container/vec.h"
# include "hook.h"
# include "buffer/line.h"

extern hook buffer_line_on_delete;
extern hook buffer_line_on_insert;
extern hook buffer_line_on_change;

extern hook buffer_on_create;
extern hook buffer_on_delete;

typedef struct buffer_s buffer;

buffer *buffer_init(void);

void buffer_free(buffer *b);

int buffer_insert(buffer *b, lineno ln);

int buffer_delete(buffer *b, lineno index);

vec *buffer_get_line(buffer *b, lineno ln);

int buffer_set_line(buffer *b, lineno ln, vec *l);

lineno buffer_len(buffer *b);

#endif /* BUFFER_CORE_H */
