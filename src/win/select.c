#include "win/util.h"
#include "win/win.h"
#include "hook.h"

#include "win/select.h"

/* Window selection is handled by each splitter window havng a selected       *
 * property. This is set to one of sub1, sub2, and none. Sub1 and sub2        *
 * indicate that either the first or second child are or contain the selected *
 * window, while none indicates that the splitter window itself is selected.  *
 *                                                                            *
 * While this is clearly slower to set and get than just having a pointer to  *
 * a selected window, it is kinda cuter, and also handles events like window  *
 * deletion without adding extra code or hooks to other parts of the window   *
 * system.                                                                    */

hook_add(win_on_select, 2);

int win_select_set(win *w)
{
    win *oldselect;

    /* Get the currently selected window to call win_on_select with */
    oldselect = win_select_get();

    /* If the window we're now selecting is a splitter, tell it that *
     * none of its children are selected.                            */
    if (win_issplit(w))
        w->cont.split.selected = none;

    /* Iterate upwards from the newly selected window to the root */
    while (! win_isroot(w))
    {
        win *par;
        /* Get the window's parent */
        par = w->parent;

        /* Set the parent's selected child correctly. */
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

    /* Start iterating from the root window */
    w = win_root;

    /* While the window is a splitter and not itself selected, *
     * iterate to its selected child                           */
    while (win_issplit(w) &&
           w->cont.split.selected != none)
    {
        if (w->cont.split.selected == sub1)
            w = w->cont.split.sub1;

        if (w->cont.split.selected == sub2)
            w = w->cont.split.sub2;
    }

    return w;
}
