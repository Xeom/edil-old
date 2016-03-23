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

static void ui_win_draw_highlight(void);
static int ui_win_draw_wintree(wintree *tree);
static int ui_win_draw_wintree_frame(wintree *tree);

int ui_win_initsys(void)
{
    ui_win_frame_sel_face      = ui_face_init(COLOR_BLACK, COLOR_WHITE);
    ui_win_frame_face          = ui_face_init(COLOR_RED, COLOR_BLACK);
    ui_win_frame_face->bright  = 1;

    return 0;
}

int ui_win_killsys(void)
{
    return 0;
}

int ui_win_draw(void)
{
    int      borderattr;
    wintree *curr;

    borderattr = ui_face_get_attr(ui_win_frame_face);
    curr       = wintree_iter_start();

    attron(borderattr);

    while (curr)
    {
        ui_win_draw_wintree(curr);
        curr = wintree_iter_next(curr);
    }

    attroff(borderattr);

    ui_win_draw_highlight();

    return 0;
}

static int ui_win_draw_wintree(wintree *tree)
{
    size_t ln;

    ui_win_draw_wintree_frame(tree);

/*
    ln = wintree_get_sizey(tree) - 1;
    while (ln--)
        ui_display_wintree_line(tree, ln);
*/
    return 0;
}

static int ui_win_draw_wintree_frame(wintree *tree)
{
    size_t posx, posy, lastposx, lastposy, sizex, sizey;
    char *caption;

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree);

    if (wintree_get_sizey(tree) == 0 || wintree_get_sizex(tree) == 0)
        return -1;

    sizex = wintree_get_sizex(tree);
    sizey = wintree_get_sizey(tree);

    lastposy = posy + sizey - 1;
    lastposx = posx + sizex - 1;

    caption = wintree_get_caption(tree);

    move(lastposy, posx);
    ui_util_draw_text_limited(sizex, caption, ui_win_horizontal_char);

    while (posy < lastposy)
        mvaddch(posy++, lastposx, ui_win_vertical_char);

    mvaddch(lastposy, lastposx, ui_win_corner_char);

    return 0;
}

static void ui_win_draw_highlight(void)
{
    size_t   posx, posy, sizex, sizey;
    wintree *selected;

    selected = wintree_get_selected();

    if (selected == NULL)
        return;

    posx  = wintree_get_posx(selected);
    posy  = wintree_get_posy(selected);
    sizex = wintree_get_sizex(selected);
    sizey = wintree_get_sizey(selected);

    ui_face_draw_at(ui_win_frame_sel_face, posx, posy + sizey - 1, sizex, 1);
    ui_face_draw_at(ui_win_frame_sel_face, posx + sizex - 1, posy, 1, sizey);
}
