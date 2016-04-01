#if !defined(IO_FILE_H)
# define IO_FILE_H
# include "head.h"

# include "container/vec.h"

int file_dump_vec(vec *v, FILE *stream);

int file_dump_buffer(buffer *b, FILE *stream);

int file_read_buffer(buffer *b, FILE *stream);

#endif /* IO_FILE_H */
