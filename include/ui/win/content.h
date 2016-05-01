#if !defined(UI_WIN_CONTENT_H)
# define UI_WIN_CONTENT_H
# include "head.h"

# include "win.h"

int ui_win_content_draw_subs(win *w);

int ui_win_content_draw(win *w);

int ui_win_content_draw_lines_after(win *w, lineno ln);

int ui_win_content_draw_line(win *w, lineno ln);

#endif /* UI_WIN_CONTENT_H */
