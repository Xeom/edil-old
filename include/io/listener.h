#if !defined(IO_LISTENER_H)
# define IO_LISTENER_H
# include "head.h"
# include <stddef.h>
# include <stdio.h>

typedef struct listener_s listener;

typedef void (*listenf_none)(listener *);
typedef void (*listenf_char)(listener *, char);
typedef void (*listenf_str )(listener *, char *, size_t);

typedef enum
{
    read_char = 0x01, /* Read only single characters from the stream     */
    read_line = 0x02, /* Read sections of the line delimited by newlines */
    read_full = 0x03, /* Read all availiable characters                  */
    read_none = 0x04
} listen_type;

int io_listener_initsys(void);

listener *io_listener_init(FILE *stream, listen_type type, size_t limit,
                    listenf_char charf, listenf_str strf, listenf_none nonef);

void io_listener_free(listener *li);

int io_listener_listen(void);

#endif /* IO_LISTENER_H */
