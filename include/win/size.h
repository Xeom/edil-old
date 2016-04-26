#if !defined(WIN_SIZE_H)
# define WIN_SIZE_H
# include "head.h"

# include "win/win.h"

extern hook win_size_on_adj_pre;
extern hook win_size_on_adj_post;

int win_size_set_root(uint x, uint y);

int win_size_adj_splitter(win *w, int adj);

/* These functions proportionally resize a window.       *
 * The window's actual size should not be changed first. *
 * These functions should be called before a window is   *
 * resized, to make all its internals proprtional.       */
int win_size_resize_x(win *w, uint newsize);
int win_size_resize_y(win *w, uint newsize);

uint win_size_get_x(win *w);
uint win_size_get_y(win *w);

#endif /* WIN_SIZE_H */
