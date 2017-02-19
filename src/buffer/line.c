#include <stdlib.h>
#include <string.h>

#include "container/vec.h"
#include "buffer/chunk.h"
#include "ui/text.h"
#include "hook.h"
#include "err.h"

#include "buffer/line.h"

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

    free(l->text);
    free(l);
}

vec *buffer_line_get_vec(line *l)
{
    size_t len;
    vec *rtn;

    ASSERT_PTR(l, high, return NULL);

    /* Make a new vector to return */
    TRACE_PTR(rtn = vec_init(sizeof(char)),
              return NULL);

    len = l->length;

    /* If there's something in the line, (So the pointer is *
     * nonnull, insert the line's contents into the new vec */
    if (len)
        ASSERT_INT(vec_insert(rtn, 0, len, l->text),
                   critical, return NULL);

    return rtn;
}

vec *buffer_line_get_vec_chars(line *l)
{
    vec    *rtn;
    int32_t chr;
    char   *text, *end;

    rtn = vec_init(sizeof(int32_t));

    end  = l->text + l->length;

    if (l->length)
        text = ui_text_first_char(l->text, end);
    else
        text = NULL;

    while (text)
    {
        chr = ui_text_decode_utf8(text, end);
        vec_insert_end(rtn, 1, &chr);
        text = ui_text_next_char(text, end);
    }

    return rtn;
}

int buffer_line_set_vec(line *l, vec *v)
{
    size_t len;

    ASSERT_PTR(l, high, return -1);

    len = vec_len(v);

    if (len == 0)
    {
        /* If there was aready some text here, free it */
        if (l->text)
            free(l->text);

        l->text = NULL;
    }
    else
        /* Resize line->text to fit the new text */
        ASSERT_PTR(l->text   = realloc(l->text, len),
                   terminal, return -1);

    l->length = len;

    /* Note that we do not memcpy or alloc space for  *
     * a terminating \0. This is because we are evil. */
    if (len)
        memcpy(l->text, vec_item(v, 0), len);

    return 0;
}

int buffer_line_set_vec_chars(line *l, vec *v)
{
    size_t textind, len;

    len = vec_len(v);
    l->length = len;

    if (len)
        l->text = realloc(l->text, len);
    else
        l->text = NULL;

    textind = 0;

    vec_foreach(
        v, int32_t, chr,

        size_t symbollen;
        char   symbol[4];

        symbollen = ui_text_encode_utf8(chr, symbol);

        if (symbollen == 1)
            l->text[textind++] = *symbol;

        else if (symbollen == 0)
            l->text[textind++] = '?';

        else
        {
            l->length += symbollen - 1;
            l->text    = realloc(l->text, l->length);
            memcpy(l->text + textind, symbol, symbollen);
            textind += symbollen;
        }
    );

    return 0;
}

size_t buffer_line_len(line *l)
{
    ASSERT_PTR(l, high, return INVALID_INDEX);

    if (l->text)
        return ui_text_len(l->text, l->text + l->length);

    else
        return 0;
}
