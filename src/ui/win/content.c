#include <curses.h>

#include "buffer/buffer.h"
#include "win/win.h"
#include "win/iter.h"
#include "win/pos.h"
#include "win/size.h"
#include "ui/win/win.h"
#include "ui/util.h"

#include "ui/win/content.h"

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

    b    = win_get_buffer(w);

    numlines = buffer_len(b);

    offx = win_get_offsetx(w);
    offy = win_get_offsety(w);

    curry = win_pos_get_y(w) + (int)(ln - offy);
    currx = win_pos_get_x(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    if (ln >= offy + sizey - 1)
        return 0;

    if (ln >= numlines)
        l = NULL;
    else
    {
        l = buffer_get_line(b, ln);
        vec_delete(l, 0, MIN(offx, vec_len(l)));
        hook_call(ui_win_content_on_draw_line_pre, w, b, &ln, l);
    }

    move(curry++, currx);

    if (l)
        ui_util_draw_text_limited_h(sizex - 1, (uint)vec_len(l), ' ', vec_item(l, 0));
    else
        ui_util_draw_text_limited_h(sizex - 1, 0, ' ', "");

    hook_call(ui_win_content_on_draw_line_post, &w, &b, &ln, &l);

    vec_free(l);

    return 0;
}
