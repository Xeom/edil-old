#if !defined(WIN_SELECT_H)
# define WIN_SELECT_H
# include "head.h"

# include "win/win.h"

/* Hook called when a new window is selected.                              *
 * Called with two parameters, the newly selected window, and the old one. */
extern hook win_on_select;

/*
 * Select a window.
 *
 * @param w A pointer to the window to select.
 *
 * @return  0 on success, -1 on failure.
 *
 */
int win_select_set(win *w);

/*
 * Get the currently selected window.
 *
 * @return A pointer to the window.
 *
 */
win *win_select_get(void);

#endif /* WIN_SELECT_H */
