#if defined(__unix__)
/* Check fileno requirements */
# if _POSIX_C_SOURCE >= 1 || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#  include <stdio.h>
# else
/* This is required to make stdio provide fileno when compiled without
 * posix extentions (--std=c* not --std=gnu*) or with pedantic or strict
 * modes. */
#  define _POSIX_SOURCE
#  include <stdio.h>
#  undef  _POSIX_SOURCE
# endif
# include <sys/select.h>
# include <unistd.h>
#else
# error "I still hate you"
#endif

#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "io/listener.h"

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

#define LISTENER_EOF(li) (feof((li).stream))
#define LISTENER_FD(li) (fileno((li).stream))

static vec listeners;

static int io_listener_fill_set(fd_set *set);
static int io_listener_read(listener *li);
static int io_listener_update_set(fd_set *set, int numbits);

/* Should be moved from here someday */
#include <ncurses.h>
#include "io/key.h"
#include "ui/ui.h"
static void io_listener_handle_chr_wrap(listener *li)
{
    int ch;
    ch = getch();

    if (ch != KEY_RESIZE)
        io_key_handle_chr(ch);
}

int io_listener_initsys(void)
{
    vec_create(&listeners, sizeof(listener));


    io_listener_init(stdin, read_none, 0,
                     NULL, NULL, io_listener_handle_chr_wrap);

    return 0;
}

void io_listener_free(listener *li)
{
    void *first;
    ptrdiff_t offset;
    size_t    index;

    first = vec_item(&listeners, 0);
    ASSERT((void *)li >= first, high, return);

    offset = (char *)li - (char *)first;
    index  = (size_t)offset / sizeof(listener);
    ASSERT(((size_t)offset) % sizeof(listener) == 0, high, return);

    vec_delete(&listeners, index, 1);
}

listener *io_listener_init(FILE *stream, listen_type type, size_t limit,
                       listenf_char charf, listenf_str strf, listenf_none nonef)
{
    listener new;

    new.stream = stream;
    new.type   = type;
    new.limit  = limit;

    if (type == read_char)
    {
        ASSERT(charf && !strf && !nonef, high, return NULL);
        new.funct.charf = charf;
    }

    if (type == read_line || type == read_full)
    {
        ASSERT(strf && !charf && !nonef, high, return NULL);
        new.funct.strf = strf;
    }

    if (type == read_none)
    {
        ASSERT(nonef && !strf && !charf, high, return NULL);
        new.funct.nonef = nonef;
    }

    TRACE_INT(vec_insert_end(&listeners, 1, &new),
              return NULL);

    return vec_item(&listeners, vec_len(&listeners) - 1);
}

static int io_listener_fill_set(fd_set *set)
{
    int maxfd;

    FD_ZERO(set);
    maxfd = 0;

    vec_foreach(&listeners, listener, li,
                int fd;

                fd = LISTENER_FD(li);
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

        chars = vec_init(1);
        for (;;)
        {
            int  chr;
            char chr_cast;

            chr = fgetc(li->stream);

            if (chr == EOF)
                break;

            chr_cast = (char)chr;
            vec_insert_end(chars, 1, &chr_cast);

            if (chr == '\n')
                break;

            if (li->limit && vec_len(chars) >= li->limit)
                break;
        }

        li->funct.strf(li, vec_item(chars, 0), vec_len(chars));
        vec_free(chars);
    }

    else if (li->type == read_full)
    {
        size_t size;
        char  *buf;
        size = 1;
        buf  = NULL;

        for (;;)
        {
            size_t numread;
            buf     = realloc(buf, size);
            numread = fread(buf, 1, size, li->stream);

            if (numread < size)
            {
                size += numread;
                break;
            }

            if (size == li->limit)
                break;

            size = MIN(size << 1, li->limit);
        }

        li->funct.strf(li, buf, size);
/* TODO: ERRAHS */
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

        li = vec_item(&listeners, i % len);
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
