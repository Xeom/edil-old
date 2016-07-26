#if !defined(WIN_ITER_H)
# define WIN_ITER_H
# include "head.h"

# include "win/win.h"

/*
 * This function returns the first leaf subwindow of a window. If the window is
 * a leaf, it returns the window itself. The first window is the sub1 of the
 * sub1 of the sub1 ... of the window handed to the function. This makes it
 * visually the top-left.
 *
 * @param w A pointer to a window to get the first subwindow of.
 *
 * @return  A pointer to the first subwindow.
 *
 */
win *win_iter_first(win *w);

/*
 * This function returns the last leaf subwindow of a window. If the window is a
 * leaf, it returns the window itself. The last window is the sub2 of the sub2
 * of the sub2 ... of the window handed to the function. This makes it visually
 * the bottom-right.
 *
 * @param w A pointer to a window to get the last subwindow of.
 *
 * @return  A pointer to the first subwindow.
 *
 */
win *win_iter_last(win *w);

/*
 * Get the 'next' leaf window after a given one. This function, when called on
 * its own results, will iterate over all windows. If you begin iterating at the
 * win_iter_first of a window, all its subs will be iterated over, ending with
 * its win_iter_last. This allows iteration over all the subs of a window. The
 * windows kinda iterate from top-left to bottom-right. When the function is
 * handed the win_iter_last of the root window, it returns the win_iter_first of
 * the root.
 *
 * @param w A pointer to a window.
 *
 * @return  A pointer to the next window after the provided one.
 *
 */
win *win_iter_next(win *w);

/*
 * The inverse of win_iter_next.
 *
 * @param w A pointer to a window.
 *
 * @return  A pointer to the previous window before the provided one.
 *
 */
win *win_iter_prev(win *w);

#endif /* WIN_ITER_H */
