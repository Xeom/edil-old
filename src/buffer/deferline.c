#include <stdlib.h>
#include <string.h>

#include "container/table.h"
#include "ui/win/content.h"
#include "ui/text.h"
#include "err.h"

#include "buffer/deferline.h"

struct deferline_s
{
    vec   *insertindices;
    vec   *deleteindices;
    vec   *v;
    table *inserts; /* (int) -> char* */
};

static void buffer_deferline_handle_draw_line(vec *args, hook h);

hook_add(buffer_deferline_on_draw, 4);

void buffer_deferline_free(deferline *dl)
{
    vec_rforeach(dl->insertindices, size_t, ind,
                 char  *str;

                 str = *(char **)table_get(dl->inserts, &ind);
                 free(str);
        );

    table_free(dl->inserts);
    vec_free(dl->insertindices);
    vec_free(dl->deleteindices);

    free(dl);
}

size_t buffer_deferline_invalid_index = INVALID_INDEX;

deferline *buffer_deferline_init(vec *v)
{
    deferline *rtn;

    rtn = malloc(sizeof(deferline));

    rtn->v = v;
    rtn->inserts = table_init(sizeof(char *), sizeof(int), NULL, NULL,
                              (char *)&buffer_deferline_invalid_index);
    rtn->insertindices = vec_init(sizeof(size_t));
    rtn->deleteindices = vec_init(sizeof(size_t));

    return rtn;
}

static void buffer_deferline_sorted_vinsert(vec *v, size_t value)
{
    size_t len, ind;

    len = vec_len(v);
    ind = 0;

    while (ind < len)
    {
        if (*(size_t *)vec_item(v, ind) >= value)
            break;

        ind++;
    }

    vec_insert(v, ind, 1, &value);
/* Bisearch is not very useful here, so meh
        swidth |= swidth >> 16;
        swidth |= swidth >> 8;
        swidth |= swidth >> 4;
        swidth |= swidth >> 2;
        swidth |= swidth >> 1;
        swidth += 1;

        while (swidth >>= 1)
        {
            if (sind + swidth - 1 >= slen)
                continue;

            if (*(size_t *)vec_item(dl->insertindices, sind + swidth - 1)
                <= index)
                sind += swidth;
        }
*/
}

int buffer_deferline_insert_at_byte(
    deferline *dl, size_t index, const char *str)
{
    char *new, **oldptr;
    size_t inslen, oldlen;

    if (index > vec_len(dl->v))
        return -1;

    inslen = strlen(str);
    oldptr = table_get(dl->inserts, &index);

    if (!oldptr)
    {
        static char *nul = NULL;
        table_set(dl->inserts, &index, &nul);
        oldptr  = table_get(dl->inserts, &index);

        buffer_deferline_sorted_vinsert(dl->insertindices, index);
        oldlen  = 0;
    }
    else
        oldlen = strlen(*oldptr);

    new     = realloc(*oldptr, oldlen + inslen + 1);
    *oldptr = new;

    memcpy(new + oldlen, str, inslen + 1);

    return 0;
}

int buffer_deferline_insert(deferline *dl, size_t index, const char *str)
{
    size_t byte, len;
    char *start, *end, *chr;

    len = vec_len(dl->v);

    if (len == 0)
    {
        TRACE_INT(buffer_deferline_insert_at_byte(dl, index, str), return -1);
        return 0;
    }

    start = vec_item(dl->v, 0);
    end   = start + vec_len(dl->v);

    TRACE_PTR(chr = ui_text_get_char(start, end, index), return -1);

    byte = (size_t)(chr - start);

    TRACE_INT(buffer_deferline_insert_at_byte(dl, byte, str), return -1);

    return 0;
}

int buffer_deferline_delete(deferline *dl, size_t index)
{
    if (index > vec_len(dl->v))
        return -1;

    buffer_deferline_sorted_vinsert(dl->deleteindices, index);

    return 0;
}

vec *buffer_deferline_get_vec(deferline *dl)
{
    return dl->v;
}

#define vec_last(v) *(size_t *)vec_item(v, vec_len(v) - 1)
static int buffer_deferline_insert_next(deferline *dl)
{
    char  *str;
    size_t len, index;

    index = vec_last(dl->insertindices);
    str   = *(char **)table_get(dl->inserts, &index);
    len   = strlen(str);

    vec_insert(dl->v, index, len, str);

    vec_delete(dl->insertindices, vec_len(dl->insertindices) - 1, 1);

    return 0;
}

static int buffer_deferline_delete_next(deferline *dl)
{
    size_t index;

    index = vec_last(dl->deleteindices);
    vec_delete(dl->v, index, 1);

    vec_delete(dl->deleteindices, vec_len(dl->deleteindices) - 1, 1);

    return 0;
}

#define vec_gt(a, b) vec_len(a) && (vec_len(b) == 0 || vec_last(a) > vec_last(b))
int buffer_deferline_dump(deferline *dl)
{
    while (vec_len(dl->insertindices) ||
           vec_len(dl->deleteindices))
    {
        if (vec_gt(dl->insertindices, dl->deleteindices))
            buffer_deferline_insert_next(dl);

        else
            buffer_deferline_delete_next(dl);
    }

    return 0;
}

static void buffer_deferline_handle_draw_line(vec *args, hook h)
{
    void *w, *b, *ln;
    vec  *v;
    deferline *dl;

    w  = *(void **)vec_item(args, 0);
    b  = *(void **)vec_item(args, 1);
    ln = *(void **)vec_item(args, 2);
    v  = *(vec **)vec_item(args, 3);

    dl = buffer_deferline_init(v);

    hook_call(buffer_deferline_on_draw, w, b, ln, dl);

    buffer_deferline_dump(dl);
    buffer_deferline_free(dl);
}

int buffer_deferline_initsys(void)
{
    hook_mount(&ui_win_content_on_draw_line_pre,
               buffer_deferline_handle_draw_line, 100);

    return 0;
}
