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
#include "err.h"

#include "buffer/log.h"

static table log_points_by_listeners;

static void buffer_log_point_handle_line(listener *li, char *str, size_t n)
{
    point **p;
    int     newline;

    ASSERT_PTR(p = table_get(&log_points_by_listeners, &li),
               high, return);

    newline = 0;

    if (str[n - 1] == '\n')
    {
        newline = 1;
        str[n - 1] = '\0';
    }

    buffer_point_insert(*p, str);

    if (newline)
        buffer_point_enter(*p);
}

int buffer_log_initsys(void)
{
    table_create(&log_points_by_listeners,
                 sizeof(point *), sizeof(listener *), NULL, NULL, NULL);

    return 0;
}

FILE *buffer_log_point_stream(point *p)
{
    listener *li;
    FILE     *anus, *mouth;
    int       pipefd[2];

    pipe(pipefd);
    anus  = fdopen(pipefd[0], "r");
    mouth = fdopen(pipefd[1], "w");

    setvbuf(mouth, NULL, _IONBF, 0);
    setvbuf(anus, NULL, _IONBF, 0);

    li = io_listener_init(anus, read_line, 0, NULL,
                          buffer_log_point_handle_line, NULL);

    table_set(&log_points_by_listeners, &li, &p);

    return mouth;
}
