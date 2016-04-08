#include <stdio.h>

#if defined(__unix__)
# include <unistd.h>
#else
# error "Yea I hate you too"
#endif

#include "err.h"
#include "buffer/buffer.h"
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
        vec *l;

        l = buffer_get_line(b, currln++);

        file_dump_vec(l, stream);
    }

    return 0;
}

static int file_read_buffer_line(buffer *b, FILE *stream)
{
    char val;
    int chr, rtn;
    size_t len;
    vec *l;


    l = vec_init(sizeof(char));

    while (1)
    {
        chr = fgetc(stream);

        if (chr == EOF)
        {
            rtn = EOF;
            break;
        }

        val = (char)chr;

        vec_insert_end(l, 1, &val);

        if (chr == '\n')
        {
            rtn = 0;
            break;
        }
    }

    len = buffer_len(b);

    buffer_insert(b, len);
    buffer_set_line(b, len, l);
    vec_free(l);

    return rtn;
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
