#include <curses.h>

#include "win/util.h"

#include "win/size.h"

static uint win_size_root_x;
static uint win_size_root_y;

static int win_size_adj_splitter_lr(win *w, int adj);
static int win_size_adj_splitter_ud(win *w, int adj);

int win_size_set_root(uint x, uint y)
{
    win_size_resize_x(win_root, x);
    win_size_resize_y(win_root, y);

    win_size_root_x = x;
    win_size_root_y = y;

    return 0;
}

static int win_size_adj_splitter_lr(win *w, int adj)
{
    uint sizex, curroffset;

    curroffset = w->cont.split.sub2offset;
    sizex      = win_size_get_x(w);

    if (adj + (int)curroffset > (int)sizex ||
        adj + (int)curroffset < 0)
        return 0;

    win_size_resize_x(w->cont.split.sub1,
                      (uint)(adj + (int)curroffset));
    win_size_resize_x(w->cont.split.sub1,
                      (uint)((int)sizex - (int)curroffset - adj));

    w->cont.split.sub2offset = (uint)((int)curroffset + adj);

    return 0;
}

static int win_size_adj_splitter_ud(win *w, int adj)
{
    uint sizey, curroffset;

    curroffset = w->cont.split.sub2offset;
    sizey      = win_size_get_y(w);

    if (adj + (int)curroffset > (int)sizey ||
        adj + (int)curroffset < 0)
        return 0;

    win_size_resize_y(w->cont.split.sub1,
                      (uint)((int)curroffset + (int)adj));
    win_size_resize_y(w->cont.split.sub1,
                      (uint)((int)sizey - (int)curroffset - adj));

    w->cont.split.sub2offset = (uint)((int)curroffset + adj);

    return 0;
}

int win_size_adj_splitter(win *w, int adj)
{
    if (win_isleaf(w))
        return 0;

    if (w->type == lrsplit)
        win_size_adj_splitter_lr(w, adj);

    if (w->type == udsplit)
        win_size_adj_splitter_ud(w, adj);

    return 0;
}

int win_size_resize_x(win *w, uint newsize)
{
    if (w->type == udsplit)
    {
        win_size_resize_x(w->cont.split.sub1, newsize);
        win_size_resize_x(w->cont.split.sub2, newsize);
    }

    if (w->type == lrsplit)
    {
        float adjfract;
        uint newoffset;

        adjfract = (float)newsize / (float)win_size_get_x(w);
        newoffset = (uint)((float)w->cont.split.sub2offset * adjfract);

        win_size_resize_x(w->cont.split.sub1, newoffset);
        win_size_resize_x(w->cont.split.sub2, newsize - newoffset);

        w->cont.split.sub2offset = newoffset;
    }

    return 0;
}

int win_size_resize_y(win *w, uint newsize)
{
    if (w->type == lrsplit)
    {
        win_size_resize_y(w->cont.split.sub1, newsize);
        win_size_resize_y(w->cont.split.sub2, newsize);
    }

    if (w->type == udsplit)
    {
        float adjfract;
        uint newoffset;

        adjfract = (float)newsize / (float)win_size_get_y(w);
        newoffset = (uint)((float)w->cont.split.sub2offset * adjfract);

        win_size_resize_y(w->cont.split.sub1, newoffset);
        win_size_resize_y(w->cont.split.sub2, newsize - newoffset);

        w->cont.split.sub2offset = newoffset;
    }

    return 0;
}

uint win_size_get_x(win *w)
{
    win *par;
    par = w->parent;

    if (win_isroot(w))
        return win_size_root_x;

    if (par->type == lrsplit)
    {
        if (win_issub1(w))
            return par->cont.split.sub2offset;

        if (win_issub2(w))
            return win_size_get_x(par) -
                par->cont.split.sub2offset;
    }

    return win_size_get_x(par);
}

uint win_size_get_y(win *w)
{
    win *par;
    par = w->parent;

    if (win_isroot(w))
        return win_size_root_y;

    if (par->type == udsplit)
    {
        if (win_issub1(w))
            return par->cont.split.sub2offset;

        if (win_issub2(w))
            return win_size_get_y(par) -
                par->cont.split.sub2offset;
    }

    return win_size_get_y(par);
}
