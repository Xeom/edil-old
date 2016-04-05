#include <stdlib.h>

#include "container/vec.h"
#include "buffer/chunk.h"

#include "buffer/line.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE line *
#define VEC_TYPED_NAME lines
#include "container/typed_vec.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE char
#define VEC_TYPED_NAME char
#include "container/typed_vec.h"

struct line_s
{
    vec_char text; /* A line is an extention of a vec_char. All normal vec/vec_char operations work */
};

line *buffer_line_init(void)
{
    line *rtn = malloc(sizeof(line));

    vec_char_create(rtn);

    return rtn;
}

void buffer_line_free(line *l)
{
    vec_char_free((vec_char *)l);
}
