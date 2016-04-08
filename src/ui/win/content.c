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
    win *sub, *last;

    sub  = win_iter_first(w);
    last = win_iter_last(w);

    while (sub != last)
    {
        ui_win_content_draw(sub);
        sub = win_iter_next(sub);
    }

    ui_win_content_draw(sub);

    refresh();

    return 0;
}

int ui_win_content_draw(win *w)
{
    buffer *b;
    size_t ln, numlines;
    uint sizex, sizey;
    uint offx,  offy;
    int  currx, curry;

    b    = win_get_buffer(w);

    hook_call(ui_win_on_draw_content_pre, &w, &b);

    numlines = buffer_len(b);

    offx = win_get_offsetx(w);
    offy = win_get_offsety(w);

    curry = win_pos_get_y(w);
    currx = win_pos_get_x(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    ln = (size_t)offy;

    while (sizey--)
    {
        vec  *l;
        char *iter;

        if (ln >= numlines)
            break;

        l = buffer_get_line(b, ln);

        hook_call(ui_win_on_draw_content_line_pre, &w, &b, &ln, &l);

        if (vec_len(l) < offx)
            iter = "";
        else
            iter = vec_item(l, offx);

        move(curry++, currx);

        ui_util_draw_text_limited_h(sizex, vec_len(l) - offx, ' ', iter);

        hook_call(ui_win_on_draw_content_line_post, &w, &b, &ln, &l);

        vec_free(l);
        ln++;
    }

    hook_call(ui_win_on_draw_content_post, &w, &b);

    return 0;
}
