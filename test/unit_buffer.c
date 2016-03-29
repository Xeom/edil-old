#include <stdio.h>

#include "buffer/core.h"
#include "io/file.h"
#include "container/vec.h"

main()
{
    buffer *b;

    b = buffer_init();

    vec_insert((vec *)buffer_insert(b, 0), 0, 7, "Hello\n");
    vec_insert((vec *)buffer_insert(b, 1), 0, 8, "World.\n");
    vec_insert((vec *)buffer_insert(b, 2), 0, 2, "\n");
    vec_insert((vec *)buffer_insert(b, 3), 0, 4, "<3\n");

    file_dump_buffer(b, stdout);
    buffer_free(b);

    return 0;
}
