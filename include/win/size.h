#if !defined(WIN_SIZE_H)
# define WIN_SIZE_H
# include "head.h"

# include "win/win.h"

int win_size_set_root(uint x, uint y);

int win_size_adj_splitter(win *w, int adj);

int win_size_resize_x(win *w, uint newsize);

int win_size_resize_y(win *w, uint newsize);

uint win_size_get_x(win *w);

uint win_size_get_y(win *w);

#endif /* WIN_SIZE_H */
