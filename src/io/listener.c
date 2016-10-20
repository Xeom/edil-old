/* Check fileno requirements */
#if _POSIX_C_SOURCE >= 1 || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
# include <stdio.h>
#else
/* This is required to make stdio provide fileno when compiled without
 * posix extentions (--std=c* not --std=gnu*) or with pedantic or strict
 * modes. */
# define _POSIX_SOURCE
# include <stdio.h>
# undef  _POSIX_SOURCE
#endif

#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "io/listener.h"
#include "io/key.h"

struct listener_s
{
    FILE       *stream;
    listen_type type;

    union
    {
        listenf_char charf;
        listenf_str  strf;
        listenf_none nonef;
    } funct;

    size_t limit;
};

/* Call feof/fileno on listeners */
#define LISTENER_EOF(li) (feof((li).stream))
#define LISTENER_FD(li) (fileno((li).stream))

static vec listeners;

listener *stdin_listener;

static int io_listener_fill_set(fd_set *set);
static int io_listener_read(listener *li);
static int io_listener_update_set(fd_set *set, int numbits);

static void io_listener_handle_chr_wrap(listener *li)
{
    io_key_poll();
}

int io_listener_initsys(void)
{
    TRACE_PTR(vec_create(&listeners, sizeof(listener *)),
              return -1);

    TRACE_PTR(stdin_listener = io_listener_init(stdin, read_none, 0,
                                                NULL, NULL,
                                                io_listener_handle_chr_wrap),
              return -1);

    return 0;
}

int io_listener_free(listener *li)
{
    size_t index;

    ASSERT(vec_contains(&listeners, &li) == 1, high, return -1);

    TRACE_IND(index = vec_find(&listeners, &li), return -1);
    TRACE_INT(vec_delete(&listeners, index, 1),  return -1);

    free(li);

    return 0;
}

listener *io_listener_init(FILE *stream, listen_type type, size_t limit,
                       listenf_char charf, listenf_str strf, listenf_none nonef)
{
    listener *new;
    int       fd;

    /* Allocate space for listener, and set it up with params */
    ASSERT_PTR(new = malloc(sizeof(listener)),
               terminal, return NULL);

    new->stream = stream;
    new->type   = type;
    new->limit  = limit;

    /* Get the fileno of the listener */
    ASSERT_INT(fd = LISTENER_FD(*new), high,
               free(new);
               return NULL);

    /* For read_char listeners, ensure we only have a char function */
    if (type == read_char)
    {
        ASSERT(charf && !strf && !nonef, high,
               free(new);
               return NULL);

        new->funct.charf = charf;
    }

    /* Both read_line and read_full listeners use the same str functions.     *
     * Check we only have those. Also force the stream into nonblocking mode. */
    else if (type == read_line || type == read_full)
    {
        ASSERT(strf && !charf && !nonef, high,
               free(new);
               return NULL);

        new->funct.strf = strf;

        /* Instead of setting all new flags, or O_NONBLOCK to existing ones */
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    }

    else if (type == read_none)
    {
        ASSERT(nonef && !strf && !charf, high,
               free(new);
               return NULL);
        new->funct.nonef = nonef;
    }

    else
    {
        ERR_NEW(high, "Invalid listen_type",
                "type is not a valid listen_type");
        free(new);
        return NULL;
    }

    /* Try to add the new listener to the vec of all listeners */
    TRACE_INT(vec_insert_end(&listeners, 1, &new),
              free(new);
              return NULL);

    return new;
}

static int io_listener_fill_set(fd_set *set)
{
    int maxfd;

    FD_ZERO(set);
    maxfd = 0;

    vec_foreach(&listeners, listener *, li,
                int fd;

                ASSERT_INT(fd = LISTENER_FD(*li),
                           critical, continue);

                maxfd = MAX(maxfd, fd);
                FD_SET(fd, set);
        );

    return maxfd;
}

static int io_listener_read(listener *li)
{
    if (li->type == read_char)
    {
        int chr;

        chr = fgetc(li->stream);

        ASSERT(chr != EOF, high,
               return -1);

        li->funct.charf(li, (char)chr);
    }
/* READ_LINE AND READ_FULL ARE UNTESTED AND LIKELY ON FIRE (!!!) */
    else if (li->type == read_line)
    {
        vec *chars;
        char nullterm;

        nullterm = '\0';

        chars = vec_init(1);
        for (;;)
        {
            int  chr;
            char chr_cast;

            chr = fgetc(li->stream);

            if (chr == EOF)
                break;
            #if EOF != -1
             if (chr == -1)
                 break;
            #endif
            chr_cast = (char)chr;
            vec_insert_end(chars, 1, &chr_cast);

            if (chr == '\n')
                break;

            if (li->limit && vec_len(chars) >= li->limit)
                break;
        }

        vec_insert_end(chars, 1, &nullterm);
        li->funct.strf(li, vec_item(chars, 0), vec_len(chars) - 1);
        vec_free(chars);
    }

    else if (li->type == read_full)
    {
        size_t totalread, bufsize;
        char  *buf;

        totalread = 0;
        bufsize   = 8;
        buf       = NULL;

        for (;;)
        {
            size_t numread;
            size_t numreq;
            buf     = realloc(buf, bufsize + 1);
            numreq  = bufsize - totalread;
            numread = fread(buf, 1, numreq, li->stream);

            totalread += numread;

            if (numread < numreq)
                break;

            if (bufsize == li->limit)
                break;

            bufsize = MIN(bufsize << 1, li->limit);
        }

        buf[totalread] = '\0';
        li->funct.strf(li, buf, totalread);

        free(buf);
    }

    else if (li->type == read_none)
    {
        li->funct.nonef(li);
    }

    return 0;
}

static int io_listener_update_set(fd_set *set, int numbits)
{
    size_t i, len;
    static size_t last;

    len = vec_len(&listeners);

    for (i = last; i < len + last; ++i)
    {
        listener *li;

        li = *(listener **)vec_item(&listeners, i % len);
        if (FD_ISSET(LISTENER_FD(*li), set))
        {
            io_listener_read(li);

            if ((--numbits) <= 0)
                break;
        }
    }

    last =  i % len;

    return 0;
}

int io_listener_listen(void)
{
    int    numbits;
    fd_set set;

    numbits = select(io_listener_fill_set(&set) + 1,
                     &set, NULL, NULL, NULL);

    io_listener_update_set(&set, numbits);

    return 0;
}
