#include <curses.h>

#include "buffer/buffer.h"
#include "wintree.h"
#include "ui/util.h"
#include "wintree.h"

#include "ui/win/content.h"

int ui_win_content_draw_subs(wintree *tree)
{
    wintree *iter, *end, *next;

    iter = wintree_iter_subs_start(tree);
    end  = wintree_iter_subs_end(tree);

    next = iter;

    do
    {
        ui_win_content_draw(iter = next);
        next = wintree_iter_next(iter);
    }
    while (iter != end);

    refresh();

    return 0;
}

int ui_win_content_draw(wintree *tree)
{
    buffer *b;
    size_t ln;
    uint sizex, sizey;
    int  currx, curry;

    b     = wintree_get_buffer(tree);

    ln    = wintree_get_offsetline(tree);

    curry = wintree_get_posy(tree);
    currx = wintree_get_posx(tree);

    sizex = wintree_get_sizex(tree);
    sizey = wintree_get_sizey(tree);

    while (sizey--)
    {
        line *l;
        l = buffer_get_line(b, ln++);

        move(curry++, currx);
        ui_util_draw_vec_limited_h(sizex, ' ', (vec *)l);
    }

    return 0;
}
