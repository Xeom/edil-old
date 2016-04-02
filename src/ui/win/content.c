#include <curses.h>

#include "buffer/buffer.h"
#include "win/win.h"
#include "win/iter.h"
#include "win/pos.h"
#include "win/size.h"
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

int ui_win_content_draw(win *tree)
{
    buffer *b;
    size_t ln;
    uint sizex, sizey;
    uint offx,  offy;
    int  currx, curry;

    b     = win_get_buffer(tree);

    offx = win_get_offsetx(tree);
    offy = win_get_offsety(tree);

    curry = win_pos_get_y(tree);
    currx = win_pos_get_x(tree);

    sizex = win_size_get_x(tree);
    sizey = win_size_get_y(tree);

    ln = (size_t)offy;

    while (sizey--)
    {
        line *l;
        char *iter;

        l = buffer_get_line(b, ln++);

        if (vec_len((vec *)l) < offx)
            iter = "";
        else
            iter = vec_item((vec *)l, offx);

        move(curry++, currx);
        ui_util_draw_str_limited_h(sizex, ' ', iter);
    }

    return 0;
}
