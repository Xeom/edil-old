#include <curses.h>

#include "ui/win/frame.h"
#include "ui/win/content.h"
#include "callback.h"

#include "ui/win/win.h"

static int  ui_win_draw_frame(wintree *tree);

int ui_win_initsys(void)
{
    ui_win_frame_sel_face      = ui_face_init(COLOR_BLACK, COLOR_WHITE);
    ui_win_frame_face          = ui_face_init(COLOR_RED, COLOR_BLACK);
    ui_win_frame_face->bright  = 1;

    return 0;
}

int ui_win_killsys(void)
{
    ui_face_free(ui_win_frame_sel_face);
    ui_face_free(ui_win_frame_face);

    return 0;
}

int ui_win_draw(void)
{
    ui_win_content_draw_sub(wintree_root);

    return ui_win_draw_subframes(wintree_root);
}

int ui_win_draw_sub(wintree *tree)
{
    ui_win_content_draw_sub(tree);

    return ui_win_draw_subframes(tree);
}
