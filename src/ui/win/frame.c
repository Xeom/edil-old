#include <curses.h>

#include "ui/face.h"
#include "ui/util.h"
#include "wintree.h"

#include "ui/win/frame.h"

char ui_win_vertical_char    = '|';
char ui_win_horizontal_char  = '-';
char ui_win_corner_char      = '\'';

face *ui_win_frame_face;
face *ui_win_frame_sel_face;

int ui_win_frame_faceify(wintree *tree, face *f)
{
    int   posx,  posy;
    uint  sizex, sizey;

    if (selected == NULL)
        return -1;

    posx  = wintree_get_posx(tree);
    posy  = wintree_get_posy(tree);

    sizex = wintree_get_sizex(tree);
    sizey = wintree_get_sizey(tree);

    ui_face_draw_at(f, posx, posy + (int)sizey - 1, sizex, 1);
    ui_face_draw_at(f, posx + (int)sizex - 1, posy, 1, sizey);

    refresh();

    return 0;
}

int ui_win_frame_draw_subs(wintree *tree)
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

    ui_win_frame_faceify(wintree_get_selected(),
                         ui_win_frame_sel_face);

    return 0;
}

int ui_win_frame_draw(wintree *tree)
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
    ui_util_draw_str_limited_h(sizex - 1, ui_win_horizontal_char, caption);

    move(posy, lastposx);
    ui_util_draw_str_limited_v(sizey - 1, ui_win_vertical_char, sidebar);

    mvaddch(lastposy, lastposx, (uchar)ui_win_corner_char);

    return 0;
}
