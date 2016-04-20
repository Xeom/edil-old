#include "win/util.h"
#include "win/win.h"
#include "hook.h"

#include "win/select.h"

hook_add(win_on_select, 2);

int win_select_set(win *w)
{
    win *oldselect;

    oldselect = win_select_get();

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

    hook_call(win_on_select, win_select_get(), oldselect);

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
