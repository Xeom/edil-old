#include <curses.h>

#include "ui/face.h"
#include "ui/util.h"
#include "wintree.h"
#include "callback.h"

#include "ui/win.h"

char ui_win_left_char        = '\253';
char ui_win_right_char       = '\273';
char ui_win_vertical_char    = '|';
char ui_win_horizontal_char  = '-';
char ui_win_corner_char      = '\'';

face *ui_win_frame_face;
face *ui_win_frame_sel_face;

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
    int sizex, sizey;

    getmaxyx(stdscr, sizey, sizex);

    ui_util_clear_area(0, 0, (uint)sizex, (uint)sizey);

    return ui_win_draw_subframes(wintree_root);
}

int ui_win_draw_sub(wintree *tree)
{
    int  posx,  posy;
    uint sizex, sizey;

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree);
    sizex = wintree_get_sizex(tree);
    sizey = wintree_get_sizey(tree);

    ui_util_clear_area(posx, posy, sizex, sizey);

    return ui_win_draw_subframes(tree);
}

int ui_win_draw_subframes(wintree *tree)
{
    int borderattr;
    wintree *iter, *end, *next;

    iter = wintree_iter_subs_start(tree);
    end  = wintree_iter_subs_end(tree);

    next = iter;

    borderattr = ui_face_get_attr(ui_win_frame_face);
    attron(borderattr);

    do
    {
        ui_win_draw_frame(iter = next);
        next = wintree_iter_next(iter);
    }
    while (iter != end);

    attroff(borderattr);

    ui_win_draw_highlight(ui_win_frame_sel_face);

    refresh();

    return 0;
}

static int ui_win_draw_frame(wintree *tree)
{
    int     posx,     posy;
    int lastposx, lastposy;
    uint   sizex,    sizey;

    char *caption, *sidebar;

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree);

    if (wintree_get_sizey(tree) == 0 || wintree_get_sizex(tree) == 0)
        return -1;

    sizex = wintree_get_sizex(tree);
    sizey = wintree_get_sizey(tree);

    lastposy = posy + (int)sizey - 1;
    lastposx = posx + (int)sizex - 1;

    caption = wintree_get_caption(tree);
    sidebar = wintree_get_sidebar(tree);

    move(lastposy, posx);
    ui_util_draw_text_limited_h(sizex - 1, caption, ui_win_horizontal_char);

    move(posy, lastposx);
    ui_util_draw_text_limited_v(sizey - 1, sidebar, ui_win_vertical_char);

    mvaddch(lastposy, lastposx, (uchar)ui_win_corner_char);

    return 0;
}

void ui_win_draw_highlight(face *f)
{
    int   posx,  posy;
    uint  sizex, sizey;
    wintree *selected;

    selected = wintree_get_selected();

    if (selected == NULL)
        return;

    posx  = wintree_get_posx(selected);
    posy  = wintree_get_posy(selected);

    sizex = wintree_get_sizex(selected);
    sizey = wintree_get_sizey(selected);

    ui_face_draw_at(f, posx, posy + (int)sizey - 1, sizex, 1);
    ui_face_draw_at(f, posx + (int)sizex - 1, posy, 1, sizey);
}
