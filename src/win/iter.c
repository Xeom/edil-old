#include "win/util.h"

#include "win/iter.h"

win *win_iter_first(win *w)
{
    /* Go down the sub1s for every window */
    while (win_issplit(w))
        w = w->cont.split.sub1;

    return w;
}

win *win_iter_last(win *w)
{
    /* Go down the sub2s for every window */
    while (win_issplit(w))
        w = w->cont.split.sub2;

    return w;
}

win *win_iter_next(win *w)
{
    /* It's best to imagine this as counting in binary. Since the windows are *
     * a bitree, imagine every sub2 as a 1, and every sub1 as a 0.            */

    /* Just like binary, get rid of the longest continuous line of trailing 1 */
    while (win_issub2(w))
        w = w->parent;

    /* Find the sub2 sister of the first sub1 you find in the tree. */
    if (!win_isroot(w))
        w = w->parent->cont.split.sub2;

    /* Get the first subwindow in it. */
    return win_iter_first(w);
}

win *win_iter_prev(win *w)
{
    /* An inverted version of win_iter_next */
    while (win_issub1(w))
        w = w->parent;

    if (!win_isroot(w))
        w = w->parent->cont.split.sub1;

    return win_iter_last(w);
}
