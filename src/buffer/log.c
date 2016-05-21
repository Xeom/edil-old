#if defined(__unix__)
/* Check fdopen requirements */
# if _POSIX_C_SOURCE >= 1 || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#  include <stdio.h>
# else
/* This is required to make stdio provide fdopen when compiled without
 * posix extentions (--std=c* not --std=gnu*) or with pedantic or strict
 * modes. */
#  define _POSIX_SOURCE
#  include <stdio.h>
#  undef  _POSIX_SOURCE
# endif
# include <unistd.h>
#else
# error "I still hate you"
#endif

#include "io/listener.h"
#include "container/table.h"
#include "buffer/buffer.h"

#include "buffer/log.h"

static table log_buffers_by_listeners;

static void buffer_log_handle_line(listener *li, char *str, size_t n)
{
    buffer **b;
    vec    *l;

    b = table_get(&log_buffers_by_listeners, &li);

    if (str[n - 1] == '\n')
        n--;

    l = vec_init(1);
    vec_insert(l, 0, n, str);

    buffer_insert(*b, 0);
    buffer_set_line(*b, 0, l);

    vec_free(l);
}

int buffer_log_initsys(void)
{
    table_create(&log_buffers_by_listeners,
                 sizeof(buffer *), sizeof(listener *), NULL, NULL, NULL);

    return 0;
}

FILE *buffer_log_stream(void)
{
    listener *li;
    buffer *b;
    int pipefd[2];

    FILE *anus, *mouth;

    b = buffer_init();

    pipe(pipefd);
    anus  = fdopen(pipefd[0], "r");
    mouth = fdopen(pipefd[1], "w");

    li = io_listener_init(anus, read_line, 0, NULL, buffer_log_handle_line, NULL);

    table_set(&log_buffers_by_listeners, &li, &b);

    return mouth;
}
