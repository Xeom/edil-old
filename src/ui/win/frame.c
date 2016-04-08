#include <stdio.h>
#include <curses.h>

#include "ui/util.h"
#include "win/pos.h"
#include "win/size.h"
#include "win/label.h"
#include "win/select.h"
#include "win/iter.h"
#include "ui/win/win.h"

#include "ui/win/frame.h"

char ui_win_vertical_char    = '|';
char ui_win_horizontal_char  = '-';
char ui_win_corner_char      = '\'';

face *ui_win_frame_face;
face *ui_win_frame_sel_face;

int ui_win_frame_faceify(win *w, face *f)
{
    int   posx,  posy;
    uint  sizex, sizey;

    if (f == NULL)
        return -1;

    posx  = win_pos_get_x(w);
    posy  = win_pos_get_y(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    ui_face_draw_at(f, posx, posy + (int)sizey - 1, sizex, 1);
    ui_face_draw_at(f, posx + (int)sizex - 1, posy, 1, sizey);

    refresh();

    return 0;
}

int ui_win_frame_draw_subs(win *w)
{
    int borderattr;
    win *sub, *last;

    sub  = win_iter_first(w);
    last = win_iter_last(w);

    borderattr = ui_face_get_attr(ui_win_frame_face);
    attron(borderattr);

    while (sub != last)
    {
        ui_win_frame_draw(sub);
        sub = win_iter_next(sub);
    }

    ui_win_frame_draw(sub);

    attroff(borderattr);

    ui_win_frame_faceify(win_select_get(),
                         ui_win_frame_sel_face);

    return 0;
}

int ui_win_frame_draw(win *w)
{
    int     posx,     posy;
    int lastposx, lastposy;
    uint   sizex,    sizey;

    const char *caption, *sidebar;

    hook_call(ui_win_frame_on_draw_pre, &w);

    posx = win_pos_get_x(w);
    posy = win_pos_get_y(w);

    if (win_size_get_y(w) == 0 || win_size_get_x(w) == 0)
        return -1;

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    lastposy = posy + (int)sizey - 1;
    lastposx = posx + (int)sizex - 1;

    caption = win_label_caption_get(w);
    sidebar = win_label_sidebar_get(w);

    move(lastposy, posx);
    ui_util_draw_str_limited_h(sizex - 1, ui_win_horizontal_char, caption);

    move(posy, lastposx);
    ui_util_draw_str_limited_v(sizey - 1, ui_win_vertical_char, sidebar);

    mvaddch(lastposy, lastposx, (uchar)ui_win_corner_char);

    hook_call(ui_win_frame_on_draw_post, &w);

    return 0;
}
