#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "container/vec.h"
#include "buffer/chunk.h"
#include "hook.h"

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
    size_t length;
    char    *text;
};

line *buffer_line_init(void)
{
    line *rtn;

    rtn = malloc(sizeof(line));

    rtn->text   = NULL;
    rtn->length = 0;

    return rtn;
}

void buffer_line_free(line *l)
{
    free(l);
}

vec *buffer_line_get_vec(line *l)
{
    size_t len;
    vec *rtn;

    rtn = vec_char_init();
    len = l->length;

    if (l->length)
        vec_char_insert(rtn, 0, len, l->text);

    return rtn;
}

int buffer_line_set_vec(line *l, vec *v)
{
    size_t len;

    len = vec_char_len(v);

    l->text = realloc(l->text, len);
    l->length = len;
    memcpy(l->text, vec_char_item(v, 0), len);

    return 0;
}
