#if !defined(UI_WIN_H)
# define UI_WIN_H
# include "head.h"

# include "win/win.h"

extern hook ui_win_frame_on_draw_pre;
extern hook ui_win_frame_on_draw_post;

extern hook ui_win_content_on_draw_pre;
extern hook ui_win_content_on_draw_post;

extern hook ui_win_content_on_draw_line_pre;
extern hook ui_win_content_on_draw_line_post;

int ui_win_initsys(void);

int ui_win_killsys(void);

int ui_win_draw(void);

int ui_win_draw_subs(win *w);

#endif /* UI_WIN_H */
