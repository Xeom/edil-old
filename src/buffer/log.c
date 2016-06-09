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

static table log_cursors_by_listeners;

static void buffer_log_point_handle_line(listener *li, char *str, size_t n)
{
    cursor **cur;
    int      newline;

    ASSERT_PTR(str, critical, return);
    ASSERT_PTR(li,  critical, return);

    /* Get the cursor associated with this listener */
    ASSERT_PTR(cur = table_get(&log_cursors_by_listeners, &li),
               high, return);

    newline = 0;

    /* We do not want to insert \n into the buffer.  *
     * This would be an escape. We remove the \n and *
     * remember it was there. Since this is a line   *
     * read, it is impossible for \n to be anywhere  *
     * but the end.                                  */
    if (str[n - 1] == '\n')
    {
        newline = 1;
        /* Pretend the string is shorter */
        str[n - 1] = '\0';
    }

    /* Insert the string, minus \n */
    TRACE_INT(cursor_insert(*cur, str),
              return);

    /* If there was a newline, insert it properly */
    if (newline)
        TRACE_INT(cursor_enter(*cur),
                  return);
}

int buffer_log_initsys(void)
{
    TRACE_PTR(table_create(&log_cursors_by_listeners,
                           sizeof(cursor *), sizeof(listener *),
                           NULL, NULL, NULL),
              return -1);

    return 0;
}

FILE *buffer_log_point_stream(cursor *cur)
{
    listener *li;
    FILE     *anus, *mouth;
    int       pipefd[2];

    fprintf(stderr, "LOGCUR %p\n", cur);

    /* Open a new pipe */
    ASSERT_ENO(pipe(pipefd) == 0, critical,
               return NULL);
    ASSERT_ENO(anus  = fdopen(pipefd[0], "r"), critical,
               close(pipefd[0]);
               close(pipefd[1]);
               return NULL);
    ASSERT_ENO(mouth = fdopen(pipefd[1], "w"), critical,
               fclose(anus);
               close(pipefd[1]);
               return NULL);

    /* Remove buffering from both ends of the pipe */
    ASSERT_ENO(setvbuf(mouth, NULL, _IONBF, 0) == 0 &&
               setvbuf(anus,  NULL, _IONBF, 0) == 0, critical,
               fclose(mouth);
               fclose(anus);
               return NULL);

    /* Hook a listener to the pipe */
    TRACE_PTR(li = io_listener_init(anus, read_line, 0, NULL,
                                    buffer_log_point_handle_line, NULL),
              fclose(mouth);
              fclose(anus);
              return NULL);

    /* Associate the cursor and listener in a table */
    TRACE_INT(table_set(&log_cursors_by_listeners, &li, &cur),
              fclose(mouth);
              fclose(anus);
              io_listener_free(li);
              return NULL);

    return mouth;
}
