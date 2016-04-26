#if !defined(WIN_ITER_H)
# define WIN_ITER_H
# include "head.h"

# include "win/win.h"

win *win_iter_first(win *w);

win *win_iter_last(win *w);

win *win_iter_next(win *w);

win *win_iter_prev(win *w);

#endif /* WIN_ITER_H */
