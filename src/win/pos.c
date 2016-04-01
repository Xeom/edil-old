#include "win/util.h"

#include "win/pos.h"

int win_pos_get_x(win *w)
{
    win *par;
    par = w->parent;

    if (win_isroot(w))
        return 0;

    if (par->type == lrsplit
        && win_issub2(w))
        return win_pos_get_x(par) +
            (int)par->cont.split.sub2offset;

    return win_pos_get_x(par);
}

int win_pos_get_y(win *w)
{
    win *par;
    par = w->parent;

    if (win_isroot(w))
        return 0;

    if (par->type == udsplit
        && win_issub2(w))
        return win_pos_get_y(par) +
            (int)par->cont.split.sub2offset;

    return win_pos_get_y(par);
}
