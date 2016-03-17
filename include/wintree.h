#ifndef WINTREE_H
# define WINTREE_H
#include "wincont.h"
#include "hook.h"

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

extern hook *wintree_on_resizex;
extern hook *wintree_on_resizey;
extern hook *wintree_on_delete;
extern hook *wintree_on_create;

/*
 * Initialize the wintree system
 *
 */
int wintree_initsys(void);

/*
 */
int wintree_killsys(void);

/*
 * Set the size of the root window of the wintree system (To be used on startup, SIGWINCH...)
 *
 * x is the number of columns of the terminal.
 *
 * y is the number of rows of the terminal.
 *
 */
int wintree_set_root_size(size_t x, size_t y);

/*
 * Remove the selected wintree and her chilren from the wintree system. Her space will be filled by her daughters.
 *
 * tree is the wintree we want to delete.
 *
 */
int wintree_delete(wintree *tree);

/*
 * Swaps the content in a wintree to the next or previous wincont in the wincont ring. Allows two windows 
 * to share a wincont.
 *
 * tree is the window whose content we wanna swap
 *
 */
int wintree_swap_next(wintree *tree);
int wintree_swap_prev(wintree *tree);

/*
 * Moves the dividing border of a splitter down or right (depending on the split direction) a set distance.
 * If it is moved too far, a window is killed.
 *
 * tree is the splitter we want to adjust.
 *
 * n is the amount of characters to move it
 *
 */
int wintree_move_border(wintree *tree, int n);

/*
 * Get the absolute (recursive sum of relative) positions of the top, left corner of wintrees
 * (measured with top left as (0, 0)) 
 *
 * tree is the wintree whose position we want to obtain
 *
 */
size_t wintree_get_posx(wintree *tree);
size_t wintree_get_posy(wintree *tree);

/*
 * Get the next tree 'after' a particular one. This allows for iteration across all content
 * (not splitter). The function returns NULL when tree is has no wintree 'after' it - It is
 * the sub2 of all its ancestors.
 *
 * tree is the current tree.
 *
 */
wintree *wintree_iter_next(wintree *tree);

/*
 *
 */
wintree *wintree_iter_start(void);

/*
 * Get the currently selected wintree.
 *
 */
wintree *wintree_get_selected(void);

/*
 * Selects the next wintree, iterating in the same manner as wintree_iter_next. Can be handed other wintrees than the
 * currently selected one, but acts oddly if it is. Maybe I'll come up with a reason to do that later.
 *
 * tree is generally the currently selected wintree.
 *
 */
int wintree_select_next(wintree *tree);

/*
 * Ensures that the parent of tree is selected instead of tree or its sister
 *
 * tree is generally the currently selected wintree.
 *
 */
int wintree_select_up(wintree *tree);

/*
 * Splits the supplied wintree in a direction. A newly cloned copy of the currently selected content is placed in
 * a new window which is the selected direction relative to a reduced size copy of the current wintree.
 *
 * tree is the window we want to split.
 *
 * dir is up/down/left/right - the direction to split in.
 *
 */
int wintree_split(wintree *tree, windir dir);

/*
 * These functions simply return a property of a wintree
 *
 */
wincont *wintree_get_content(wintree *tree);
wintree *wintree_get_parent(wintree *tree);
size_t   wintree_get_sizex(wintree *tree);
size_t   wintree_get_sizey(wintree *tree);

#endif /* WINTREE_H */
