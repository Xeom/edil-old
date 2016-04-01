#if !defined(WIN_UTIL_H)
# define WIN_UTIL_H
# include "win/struct.h"

# define win_isleaf(win)  (win->type == leaf)
# define win_issplit(win) (win->type == lrsplit || \
                           win->type == udsplit)

# define win_issub1(win) (win->parent && \
                          win->parent->cont.split.sub1 == win)
# define win_issub2(win) (win->parent && \
                          win->parent->cont.split.sub2 == win)

# define win_isroot(win) (win->parent == NULL)

#endif /* WIN_UTIL_H */
