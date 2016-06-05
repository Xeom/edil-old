#include <stdlib.h>
#include <string.h>

#include "container/table.h"
#include "ui/win/content.h"

#include "buffer/deferline.h"

struct deferline_s
{
    vec   *insertindices;
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

    return rtn;
}
#include <stdio.h>
int buffer_deferline_insert(deferline *dl, size_t index, char *str)
{
    char *new, **oldptr;
    size_t inslen, oldlen;

    if (index > vec_len(dl->v))
        return -1;

    inslen = strlen(str);

    oldptr = table_get(dl->inserts, &index);

    if (!oldptr)
    {
        size_t swidth, sind, slen;
        new = malloc(inslen + 1);

        memcpy(new, str, inslen + 1);
        table_set(dl->inserts, &index, &new);

        sind = 0;
        slen = vec_len(dl->insertindices);

        swidth  = slen;
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

        vec_insert(dl->insertindices, sind, 1, &index);
    }
    else
    {
        oldlen  = strlen(*oldptr);
        new     = realloc(*oldptr, oldlen + inslen + 1);
        *oldptr = new;

        memcpy(new + oldlen, str, inslen + 1);
    }

    return 0;
}

vec *buffer_deferline_get_vec(deferline *dl)
{
    return dl->v;
}

int buffer_deferline_dump(deferline *dl)
{
    vec_rforeach(dl->insertindices, size_t, ind,
                 char  *str;
                 size_t len;

                 str = *(char **)table_get(dl->inserts, &ind);
                 len = strlen(str);
                 vec_insert(dl->v, ind, len, str);
        );

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
