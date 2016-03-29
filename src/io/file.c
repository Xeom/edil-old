#include <stdio.h>

#ifdef __unix__
# include <unistd.h>
#else
# error "Yea I hate you too"
#endif

#include "err.h"
#include "buffer/core.h"
#include "io/file.h"

int file_dump_vec(vec *v, FILE *stream)
{
    char *str;
    str = vec_item(v, 0);

    fwrite(str, 1, vec_len(v), stream);

    return 0;
}

int file_dump_buffer(buffer *b, FILE *stream)
{
    lineno currln, numln;

    numln  = buffer_len(b);
    currln = 0;

    while (currln < numln)
    {
        line *l;

        l = buffer_get_line(b, currln++);

        file_dump_vec((vec *)l, stream);
    }

    return 0;
}

static int file_read_buffer_line(buffer *b, FILE *stream)
{
    char val;
    int chr;
    size_t len;
    line *l;

    len = buffer_len(b);
    l = buffer_insert(b, len);

    while (1)
    {
        chr = fgetc(stream);

        if (chr == EOF)
            return EOF;

        val = (char)chr;

        vec_insert_end((vec *)l, 1, &val);

        if (chr == '\n')
            return 0;
    }
}

int file_read_buffer(buffer *b, FILE *stream)
{
    int val;
    for (;;)
    {
        val = file_read_buffer_line(b, stream);

        if (val == EOF)
            return 0;

        if (val != 0)
            return -1;
    }
}
