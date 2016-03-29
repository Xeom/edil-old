#ifndef BUFFER_CHUNK_H
# define BUFFER_CHUNK_H
# include "head.h"

# include "buffer/line.h"

chunk *buffer_chunk_init(void);

size_t chunk_len(chunk *c);

void buffer_chunk_free(chunk *c);

line *buffer_chunk_insert_line(chunk *c, lineno offset);

chunk *buffer_chunk_delete_line(chunk *c, lineno offset);

line *buffer_chunk_get_line(chunk *c, lineno offset);

chunk *buffer_chunk_get_containing(chunk *c, lineno ln);

lineno buffer_chunk_lineno_to_offset(chunk *c, lineno ln);

lineno buffer_chunk_offset_to_lineno(chunk *c, lineno offset);

#endif
