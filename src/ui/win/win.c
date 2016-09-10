#include <curses.h>

#include "ui/win/frame.h"
#include "ui/win/content.h"
#include "hook.h"

#include "ui/win/win.h"

/* TODO: This */
/* (window) */
hook_add(ui_win_frame_on_draw_pre, 1);
hook_add(ui_win_frame_on_draw_post, 1);
/* (window, buffer) */
hook_add(ui_win_content_on_draw_pre, 2);
hook_add(ui_win_content_on_draw_post, 2);
/* (window, buffer, lineno, str) */
hook_add(ui_win_content_on_draw_line_pre, 4);
hook_add(ui_win_content_on_draw_line_post, 4);

int ui_win_initsys(void)
{
    ui_win_frame_sel_face    = ui_face_init();
    ui_win_frame_sel_face.fg = black;
    ui_win_frame_sel_face.bg = white;

    ui_win_frame_face        = ui_face_init();
    ui_win_frame_face.bright = 1;
    ui_win_frame_face.fg     = red;
    ui_win_frame_face.bg     = black;

    return 0;
}

int ui_win_killsys(void)
{
    return 0;
}

int ui_win_draw(void)
{
    ui_win_content_draw_subs(win_root);
    ui_win_frame_draw_subs(win_root);

    return 0;
}

int ui_win_draw_subs(win *w)
{
    ui_win_content_draw_subs(w);
    ui_win_frame_draw_subs(w);

    return 0;
}

