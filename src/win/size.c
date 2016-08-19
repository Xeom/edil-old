#include <curses.h>
#include <stdlib.h>

#include "win/util.h"
#include "err.h"

#include "win/size.h"

/* Current dimensions of the root window. */
static uint win_size_root_x;
static uint win_size_root_y;

hook_add(win_size_on_adj_pre,  2);
hook_add(win_size_on_adj_post, 2);

/* These two are called by win_size_adj_splitter depending *
 * on the type of the window being adjusted.               */
static int win_size_adj_splitter_lr(win *w, int adj);
static int win_size_adj_splitter_ud(win *w, int adj);

int win_size_set_root(uint x, uint y)
{
    /* Resizing must be done before changing root values, *
     * so resize function can get the old values.         */
    TRACE_INT(win_size_resize_x(win_root, x), return -1);
    TRACE_INT(win_size_resize_y(win_root, y), return -1);

    win_size_root_x = x;
    win_size_root_y = y;

    return 0;
}

static int win_size_adj_splitter_lr(win *w, int adj)
{
    uint sizex, curroffset;

    ASSERT_PTR(w, return -1);

    curroffset = w->cont.split.sub2offset;

    ASSERT(sizex = win_size_get_x(w),
           return -1);

    /* Do not adjust outside of range */
    adj = MIN(adj, (int)sizex - (int)curroffset);
    adj = MAX(adj, (int)curroffset);

    hook_call(win_size_on_adj_pre, w, &adj);

    /* Make the appropriate resize calls before changing the *
     * sub offset.                                           *
     * The sub1 is resized to the current offset plus plus   *
     * adjustment.                                           */

    TRACE_INT(win_size_resize_x(w->cont.split.sub1,
                                (uint)(adj + (int)curroffset)),
               return -1);
    /* The sub2 is resized to parent window size subtract    *
     * adjustment and current offset.                        */
    TRACE_INT(win_size_resize_x(w->cont.split.sub1,
                                (uint)((int)sizex - (int)curroffset - adj)),
               return -1);

    /* Change the sub offset */
    w->cont.split.sub2offset = (uint)((int)curroffset + adj);

    hook_call(win_size_on_adj_post, w, &adj);

    return 0;
}

/* Same deal as lr, but methods end in _y */
static int win_size_adj_splitter_ud(win *w, int adj)
{
    uint sizey, curroffset;

    ASSERT_PTR(w, return -1);

    curroffset = w->cont.split.sub2offset;

    ASSERT(sizey = win_size_get_y(w),
           return -1);

    if (adj + (int)curroffset > (int)sizey ||
        adj + (int)curroffset < 0)
        return 0;

    hook_call(win_size_on_adj_pre, w, &adj);

    TRACE_INT(win_size_resize_y(w->cont.split.sub1,
                                (uint)((int)curroffset + (int)adj)));
    TRACE_INT(win_size_resize_y(w->cont.split.sub1,
                                (uint)((int)sizey - (int)curroffset - adj)));

    w->cont.split.sub2offset = (uint)((int)curroffset + adj);

    hook_call(win_size_on_adj_post, w, &adj);

    return 0;
}

int win_size_adj_splitter(win *w, int adj)
{
    /* This function just automatically splits calls into lr/ud functions. *
     * this means that idiots calling it can't mess up and make everything *
     * on fire as easily.                                                  */

    /* Yea you can't fucking adjust the split ratio of a fucking leaf *
     * window can you you twat.                                       */
    if (win_isleaf(w))
        return 0;

    if (w->type == lrsplit)
        TRACE_INT(win_size_adj_splitter_lr(w, adj),
                  return -1);

    else if (w->type == udsplit)
        TRACE_INT(win_size_adj_splitter_ud(w, adj),
                  return -1);

    return 0;
}

int win_size_resize_x(win *w, uint newsize)
{
    if (w->type == udsplit)
    {
        /* If the window is up-down, each sub needs to be resized to the same *
         * width.                                                             */
        TRACE_INT(win_size_resize_x(w->cont.split.sub1, newsize), return -1);
        TRACE_INT(win_size_resize_x(w->cont.split.sub2, newsize), return -1);
    }

    if (w->type == lrsplit)
    {
        float adjfract;
        uint newoffset;

        /* Get divide the new size by the current size */
        adjfract = (float)newsize / (float)win_size_get_x(w);
        /* Multiply that by the current sub2offset to get the new sub2offset */
        newoffset = (uint)((float)w->cont.split.sub2offset * adjfract);

        /* Resize sub1 with x size of sub2's offset */
        TRACE_INT(win_size_resize_x(w->cont.split.sub1, newoffset),
                  return -1);
        /* Resize sub2 with the new x size of the window, subtract its offset */
        TRACE_INT(win_size_resize_x(w->cont.split.sub2, newsize - newoffset),
                  return -1);

        w->cont.split.sub2offset = newoffset;
    }

    return 0;
}

/* Same as above, but lr/ud and x and y swapped */
int win_size_resize_y(win *w, uint newsize)
{
    if (w->type == lrsplit)
    {
        TRACE_INT(win_size_resize_y(w->cont.split.sub1, newsize), return -1);
        TRACE_INT(win_size_resize_y(w->cont.split.sub2, newsize), return -1);
    }

    if (w->type == udsplit)
    {
        float adjfract;
        uint newoffset;

        adjfract = (float)newsize / (float)win_size_get_y(w);
        newoffset = (uint)((float)w->cont.split.sub2offset * adjfract);

        TRACE_INT(win_size_resize_y(w->cont.split.sub1, newoffset),
                  return -1);
        TRACE_INT(win_size_resize_y(w->cont.split.sub2, newsize - newoffset),
                  return -1);

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

    if (par->type != lrsplit)
        return win_size_get_x(par);

    if (win_issub1(w))
        return par->cont.split.sub2offset;

    if (win_issub2(w))
        return win_size_get_x(par) -
            par->cont.split.sub2offset;

    return 0;
}

uint win_size_get_y(win *w)
{
    win *par;
    par = w->parent;

    if (win_isroot(w))
        return win_size_root_y;

    if (par->type != udsplit)
        return win_size_get_y(par);

    if (win_issub1(w))
        return par->cont.split.sub2offset;

    if (win_issub2(w))
        return win_size_get_y(par) -
            par->cont.split.sub2offset;

    return 0;
}
