#if !defined(WIN_SELECT_H)
# define WIN_SELECT_H
# include "head.h"

# include "win/win.h"

int win_select_set(win *w);

win *win_select_get(void);

#endif /* WIN_SELECT_H */
