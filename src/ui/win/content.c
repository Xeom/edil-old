#include <curses.h>
#include <string.h>

#include "buffer/buffer.h"
#include "win/win.h"
#include "win/iter.h"
#include "win/pos.h"
#include "win/size.h"
#include "ui/win/win.h"
#include "ui/util.h"

#include "ui/win/content.h"
#include "ui/face.h"

#include "err.h"

size_t ui_win_content_get_cursor_offset(win *w, lineno ln)
{
    buffer *b;
    vec    *l;
    size_t  numlines;
    char *curiter, *limit, *iter;
    size_t index;

    b        = win_get_buffer(w);
    numlines = buffer_len(b);

    ASSERT(ln < numlines, high, return INVALID_INDEX);
    ASSERT_PTR(l = buffer_get_line(b, ln), high, return INVALID_INDEX);

    hook_call(ui_win_content_on_draw_line_pre, w, b, &ln, l);

    if (vec_len(l) == 0)
        return INVALID_INDEX;

    limit   = (char *)vec_item(l, vec_len(l) - 1) + 1;
    curiter = ui_util_text_next_face(vec_item(l, 0), limit);

    for (;;)
    {
        ASSERT_PTR(curiter, high,
                   vec_free(l);
                   return INVALID_INDEX);

        if (memcmp(curiter, face_cursor, face_serialized_len) == 0)
            break;

        curiter = ui_util_text_next_face(curiter + 1, limit);
    }

    iter = ui_util_text_next_char(vec_item(l, 0), curiter);
    index = 0;

    while (iter)
    {
        iter = ui_util_text_next_char(iter + 1, curiter);
        index++;
    }

    vec_free(l);

    return index;
}

int ui_win_content_draw_subs(win *w)
{
    win *sub, *first;

    first = win_iter_first(w);
    sub   = first;

    do
    {
        ui_win_content_draw(sub);
        sub = win_iter_next(sub);
    } while (sub != first);

    return 0;
}

int ui_win_content_draw(win *w)
{
    ulong curr, max;

    curr = win_get_offsety(w);
    max  = (ulong)win_size_get_y(w) + curr;

    for (; curr < max; ++curr)
        ui_win_content_draw_line(w, curr);

    return 0;
}

int ui_win_content_draw_lines_after(win *w, lineno ln)
{
    ulong curr, max;

    curr = MAX(ln, win_get_offsety(w));
    max  = win_size_get_y(w) + ln;

    for (; curr < max; curr++)
        ui_win_content_draw_line(w, curr);

    return 0;
}

int ui_win_content_draw_line(win *w, lineno ln)
{
    buffer *b;
    vec    *l;
    size_t  numlines;
    uint    sizex, sizey;
    ulong   offx,  offy;
    int     currx, curry;

    b        = win_get_buffer(w);
    numlines = buffer_len(b);

    offx = win_get_offsetx(w);
    offy = win_get_offsety(w);

    curry = win_pos_get_y(w) + (int)(ln - offy);
    currx = win_pos_get_x(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    if (ln >= offy + sizey - 1 || ln < offy)
        return 0;

    if (ln >= numlines)
        l = NULL;
    else
        l = buffer_get_line(b, ln);

    if (l)
    {
        hook_call(ui_win_content_on_draw_line_pre, w, b, &ln, l);

        if (vec_len(l) == 0)
            l = NULL;
    }

    if (l)
    {
        char *iter, *last;
        size_t index;

        iter = vec_item(l, 0);
        last = (char *)vec_item(l, vec_len(l) - 1) + 1;

        iter = ui_util_text_next_char(iter, last);

        for (index = 0; index < offx; index++)
        {
            iter = ui_util_text_next_char(iter + 1, last);

            if (!iter)
                return 0;
        }

        move(curry++, currx);
        ui_util_draw_text_limited_h(sizex - 1, (uint)(last - iter), ' ', iter);
    }
    else
    {
        move(curry++, currx);
        ui_util_draw_text_limited_h(sizex - 1, 0, ' ', "");
    }

    hook_call(ui_win_content_on_draw_line_post, &w, &b, &ln, &l);
// TODO Faces on offsetx
    vec_free(l);

    return 0;
}
