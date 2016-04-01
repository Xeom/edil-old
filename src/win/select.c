#include "win/util.h"
#include "win/win.h"

#include "win/select.h"

int win_select_set(win *w)
{
    if (win_issplit(w))
        w->cont.split.selected = none;

    while (! win_isroot(w))
    {
        win *par;
        par = w->parent;

        if (win_issub1(w))
            par->cont.split.selected = sub1;

        if (win_issub2(w))
            par->cont.split.selected = sub2;

        w = par;
    }

    return 0;
}

win *win_select_get(void)
{
    win *w;

    w = win_root;

    while (win_issplit(w)
           && w->cont.split.selected != none)
    {
        if (w->cont.split.selected == sub1)
            w = w->cont.split.sub1;

        if (w->cont.split.selected == sub2)
            w = w->cont.split.sub2;
    }

    return w;
}
