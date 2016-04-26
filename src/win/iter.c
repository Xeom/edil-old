#include "win/util.h"

#include "win/iter.h"

win *win_iter_first(win *w)
{
    while (win_issplit(w))
        w = w->cont.split.sub1;

    return w;
}

win *win_iter_last(win *w)
{
    while (win_issplit(w))
        w = w->cont.split.sub2;

    return w;
}

win *win_iter_next(win *w)
{
    while (win_issub2(w))
        w = w->parent;

    if (!win_isroot(w))
        w = w->parent->cont.split.sub2;

    return win_iter_first(w);
}

win *win_iter_prev(win *w)
{
    while (win_issub1(w))
        w = w->parent;

    if (!win_isroot(w))
        w = w->parent->cont.split.sub1;

    return win_iter_last(w);
}
