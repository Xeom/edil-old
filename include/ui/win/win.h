#if !defined(UI_WIN_H)
# define UI_WIN_H
# include "head.h"

# include "win/win.h"

extern hook ui_win_on_draw_frame_pre;
extern hook ui_win_on_draw_frame_post;

extern hook ui_win_on_draw_content_pre;
extern hook ui_win_on_draw_content_post;

extern hook ui_win_on_draw_content_line_pre;
extern hook ui_win_on_draw_content_line_post;

int ui_win_initsys(void);

int ui_win_killsys(void);

int ui_win_draw(void);

int ui_win_draw_subs(win *w);

#endif /* UI_WIN_H */
