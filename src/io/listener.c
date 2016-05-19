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

#include "io/key.h"
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

    int    ended;
    size_t limit;
};

#define listener_fd(li) (fileno((li).stream))
#define listener_read_full_blocksize 1024

vec listeners;

static int io_listener_fill_set(fd_set *set);
static int io_listener_read(listener *li);
static int io_listener_update_set(fd_set *set);

/* Should be moved from here someday */
#include <ncurses.h>
static void io_listener_handle_chr_wrap(void)
{
    int ch;
    ch = getch();

    io_key_handle_chr(ch);
}

int io_listener_initsys(void)
{
    vec_create(&listeners, sizeof(listener));

    io_listener_add(stdin, read_none, 0, NULL, NULL, io_listener_handle_chr_wrap);

    return 0;
}

listener *io_listener_add(FILE *stream, listen_type type, size_t limit,
                    listenf_char charf, listenf_str strf, listenf_none nonef)
{
    listener new;

    new.stream = stream;
    new.type   = type;
    new.limit  = limit;
    new.ended  = 0;

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

    vec_insert_end(&listeners, 1, &new);

    return vec_item(&listeners, vec_len(&listeners) - 1);
}

static int io_listener_fill_set(fd_set *set)
{
    int maxfd;

    FD_ZERO(set);
    maxfd = 0;

    vec_foreach(&listeners, listener, li,
                int fd;
                if (li.ended)
                    continue;

                fd = listener_fd(li);
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
               fprintf(stderr, "%p\n", (void *)li);
               li->ended = 1;
               return -1);

        li->funct.charf((char)chr);
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
            {
                li->ended = 1;
                break;
            }

            chr_cast = (char)chr;
            vec_insert_end(chars, 1, &chr_cast);

            if (chr == '\n')
                break;

            if (li->limit && vec_len(chars) >= li->limit)
                break;
        }

        li->funct.strf(vec_item(chars, 0), vec_len(chars));
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

        li->funct.strf(buf, size);
/* TODO: ERRAHS */
        free(buf);
    }
    else if (li->type == read_none)
    {
        li->funct.nonef();
    }
    return 0;
}


static int io_listener_update_set(fd_set *set)
{
    size_t i;

    for (i = 0; i < vec_len(&listeners); ++i)
    {
        listener *li;

        li = vec_item(&listeners, i);
        if (FD_ISSET(listener_fd(*li), set))
            io_listener_read(li);
    }

    return 0;
}

int io_listener_listen(void)
{
    fd_set set;

    select(io_listener_fill_set(&set) + 1, &set, NULL, NULL, NULL);
    io_listener_update_set(&set);

    return 0;
}
