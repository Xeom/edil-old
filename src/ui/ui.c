#include <curses.h>

#include "hook.h"
#include "ui/sbar.h"
#include "ui/face.h"
#include "ui/win/win.h"
#include "win/size.h"

#include "ui/ui.h"

hook_add(ui_on_resize_pre,  2);
hook_add(ui_on_resize_post, 2);

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    ui_win_initsys();
    ui_sbar_initsys();
    ui_face_initsys();

    return 0;
}

int ui_killsys(void)
{
    endwin();

    ui_win_killsys();
    ui_sbar_killsys();
    ui_face_killsys();

    return 0;
}

int ui_refresh(void)
{
    ui_draw();
    refresh();

    return 0;
}

int ui_draw(void)
{
    ui_win_draw();
    ui_sbar_draw();

    return 0;
}

int ui_resize(void)
{
    int termy, termx;

    getmaxyx(stdscr, termy, termx);

    hook_call(ui_on_resize_pre, &termy, &termx);

    win_size_set_root((uint)termx, (uint)termy - 1);

    ui_refresh();

    hook_call(ui_on_resize_post, &termy, &termx);

    return 0;
}
