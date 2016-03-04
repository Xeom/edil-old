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

int wintree_ui_display(wintree *tree);

int wintree_ui_display_root(void);

int wintree_initsys(void);

wintree *wintree_init(wincont *content);

int wintree_set_root_size(size_t x, size_t y);

wintree *wintree_get_sub_selected(wintree *tree);

wintree *wintree_delete_sub1(wintree *tree);

wintree *wintree_delete_sub2(wintree *tree);

int wintree_delete(wintree *tree);

int wintree_swap_next(wintree *tree);

int wintree_swap_prev(wintree *tree);

size_t wintree_getposx(wintree *tree);

size_t wintree_getposy(wintree *tree);

wintree *wintree_get_selected(void);

int wintree_move_border(wintree *tree, int n);

size_t wintree_getsizex(wintree *tree);

size_t wintree_getsizey(wintree *tree);

int wintree_setsizey(wintree *tree, size_t newsize);

int wintree_setsizex(wintree *tree, size_t newsize);

wintree *wintree_iter_next_content(wintree *tree);

int wintree_split(wintree *tree, windir dir);

char *wintree_get_line(wintree *tree, lineno ln);



#endif /* WINTREE_H */
