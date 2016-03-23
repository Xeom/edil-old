#include <curses.h>

#include "hook.h"
#include "wintree.h"
#include "ui/sbar.h"
#include "ui/face.h"
#include "ui/win.h"

#include "ui/core.h"

hook_add(ui_on_resize, 2);

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    ui_win_initsys();
    ui_sbar_initsys();
    ui_face_initsys();

    return 0;
}

int ui_killsys(void)
{
    endwin();

    hook_free(ui_on_resize);

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
    size_t termy, termx;

    getmaxyx(stdscr, termy, termx);

    hook_call(ui_on_resize, &termy, &termx);

    wintree_set_root_size(termx, termy - 1);

    ui_refresh();

    return 0;
}
