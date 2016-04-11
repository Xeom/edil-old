#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "container/vec.h"
#include "buffer/chunk.h"
#include "hook.h"
#include "err.h"

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

    ASSERT_PTR(rtn = malloc(sizeof(line)), terminal,
               return NULL);

    /* That entire byte of memory saving for empty lines is *
     * gonna be so useful.                                  */
    rtn->text   = NULL;
    rtn->length = 0;

    return rtn;
}

void buffer_line_free(line *l)
{
    if (!l) return;

    free(l);
}

vec *buffer_line_get_vec(line *l)
{
    size_t len;
    vec *rtn;

    ASSERT_PTR(l, high, return NULL);

    /* Make a new vector to return */
    TRACE_PTR(rtn = vec_char_init(),
              return NULL);

    len = l->length;

    /* If there's something in the line, (So the pointer is *
     * nonnull, insert the line's contents into the new vec */
    if (len)
        ASSERT_INT(vec_char_insert(rtn, 0, len, l->text),
                   critical, return NULL);

    return rtn;
}

int buffer_line_set_vec(line *l, vec *v)
{
    size_t len;

    ASSERT_PTR(l, high, return -1);

    len = vec_char_len(v);

    if (len == 0)
        l->text = NULL;
    else
        /* Resize line->text to fit the new text */
        ASSERT_PTR(l->text   = realloc(l->text, len),
                   terminal, return -1);

    l->length = len;

    /* Note that we do not memcpy or alloc space for  *
     * a terminating \0. This is because we are evil. */
    memcpy(l->text, vec_char_item(v, 0), len);

    return 0;
}
