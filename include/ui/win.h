#if !defined(UI_WIN_H)
# define UI_WIN_H

#include "wintree.h"
#include "ui/face.h"

extern face *ui_win_frame_face;
extern face *ui_win_frame_sel_face;

/*
 * Initialize the ui window system.
 *
 */
int ui_win_initsys(void);

/*
 * Kill the ui window system.
 *
 */
int ui_win_killsys(void);

/*
 *
 */
int ui_win_draw_sub(wintree *tree);

/*
 * Draw the frames of all children of a wintree (or the wintree itself, if it has no children).
 *
 * tree is the wintree to draw.
 *
 */
int ui_win_draw_subframes(wintree *tree);

/*
 * Draw all wintrees and contents
 *
 */
int ui_win_draw(void);

int ui_win_draw_highlight(face *f);

#endif /* UI_WIN_H */

