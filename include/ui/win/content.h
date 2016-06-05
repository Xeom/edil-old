#if !defined(UI_WIN_CONTENT_H)
# define UI_WIN_CONTENT_H
# include "head.h"

# include "win.h"

extern hook ui_win_content_on_draw_pre;
extern hook ui_win_content_on_draw_post;

extern hook ui_win_content_on_draw_line_pre;
extern hook ui_win_content_on_draw_line_post;

size_t ui_win_content_get_cursor_offset(win *w, lineno ln);

int ui_win_content_draw_subs(win *w);

int ui_win_content_draw(win *w);

int ui_win_content_draw_lines_after(win *w, lineno ln);

int ui_win_content_draw_line(win *w, lineno ln);

#endif /* UI_WIN_CONTENT_H */
