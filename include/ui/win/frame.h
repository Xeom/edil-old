#if !defined(WIN_FRAME_H)
# define WIN_FRAME_H
# include "head.h"

# include "ui/face.h"
# include "win/win.h"

extern face ui_win_frame_face;
extern face ui_win_frame_sel_face;

int ui_win_frame_faceify(win *w, face f);

int ui_win_frame_draw_subs(win *w);

int ui_win_frame_draw(win *w);

#endif /* WIN_FRAME_H */
