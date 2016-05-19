#if !defined(IO_LISTENER_H)
# define IO_LISTENER_H
# include "head.h"
# include <stddef.h>
# include <stdio.h>

typedef void (*listenf_none)();
typedef void (*listenf_char)(char);
typedef void (*listenf_str )(char *, size_t);

typedef struct listener_s listener;

typedef enum
{
    read_char = 0x01, /* Read only single characters from the stream     */
    read_line = 0x02, /* Read sections of the line delimited by newlines */
    read_full = 0x03, /* Read all availiable characters                  */
    read_none = 0x04
} listen_type;

int io_listener_initsys(void);

listener *io_listener_add(FILE *stream, listen_type type, size_t limit,
                    listenf_char charf, listenf_str strf, listenf_none nonef);

int io_listener_listen(void);

#endif /* IO_LISTENER_H */
