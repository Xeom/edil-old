#include <stdio.h>

#ifdef __unix__
# include <unistd.h>
#else
# error "Yea I hate you too"
#endif

#include "err.h"
#include "container/vec.h"
#include "buffer/core.h"
#include "io/file.h"

int file_dump_buffer(buffer *b, FILE *stream)
{
    lineno currln, numln;

    numln  = buffer_len(b);
    currln = 0;

    while (currln < numln)
    {
        line *l;

        l = buffer_get_line(b, currln++);
        
        fputs(vec_item((vec *)l, 0), stream);
    }

    return 0;
}

