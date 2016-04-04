#if !defined(UI_WIN_H)
# define UI_WIN_H
# include "head.h"

# include "win/win.h"

int ui_win_initsys(void);

int ui_win_killsys(void);

int ui_win_draw(void);

int ui_win_draw_subs(win *w);

#endif /* UI_WIN_H */
