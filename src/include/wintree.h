#ifndef WINTREE_H
# define WINTREE_H
#include "wincont.h"

typedef struct wintree_s wintree;

typedef enum
{
    left,
    right,
    up,
    down
} windir;

typedef enum
{
    lr,
    ud,
    none
} winsplitdir;

int wintree_initsys(void);

wintree *wintree_init(wincont *content);

int wintree_set_root_size(size_t x, size_t y);

int wintree_delete(wintree *tree);

int wintree_swap_next(wintree *tree);
int wintree_swap_prev(wintree *tree);

int wintree_move_border(wintree *tree, int n);

size_t wintree_get_posx(wintree *tree);
size_t wintree_get_posy(wintree *tree);

int wintree_set_sizey(wintree *tree, size_t newsize);
int wintree_set_sizex(wintree *tree, size_t newsize);

wintree *wintree_iter_next(wintree *tree);

wintree *wintree_get_selected(void);
int wintree_select_next(wintree *tree);

int wintree_split(wintree *tree, windir dir);

wincont *wintree_get_content(wintree *tree);
wintree *wintree_get_parent(wintree *tree);
size_t   wintree_get_sizex(wintree *tree);
size_t   wintree_get_sizey(wintree *tree);

#endif /* WINTREE_H */
