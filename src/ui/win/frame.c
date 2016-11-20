#include <curses.h>

#include "ui/util.h"
#include "win/pos.h"
#include "win/size.h"
#include "win/label.h"
#include "win/select.h"
#include "win/iter.h"
#include "ui/win/win.h"
#include "ui/text.h"

#include "ui/win/frame.h"

char ui_win_vertical_char    = '|';
char ui_win_horizontal_char  = '-';
char ui_win_corner_char      = '\'';

face ui_win_frame_face;
face ui_win_frame_sel_face;

int ui_win_frame_faceify(win *w, face f)
{
    int   posx,  posy;
    uint  sizex, sizey;

    posx  = win_pos_get_x(w);
    posy  = win_pos_get_y(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    ui_face_draw_at(f, posx, posy + (int)sizey - 1, sizex, 1);
    ui_face_draw_at(f, posx + (int)sizex - 1, posy, 1, sizey);

    return 0;
}

int ui_win_frame_draw_subs(win *w)
{
    win *sub, *first;

    first = win_iter_first(w);
    sub   = first;

    do
    {
        ui_win_frame_draw(sub);
        sub = win_iter_next(sub);
    } while (sub != first);

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

    hook_call(ui_win_frame_on_draw_pre, w);

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
    ui_text_draw_h(caption, NULL, sizex - 1, (chtype)ui_win_horizontal_char, 0);

    move(posy, lastposx);
    ui_text_draw_v(sidebar, NULL, sizey - 1, (chtype)ui_win_vertical_char, 0);

    mvaddch(lastposy, lastposx, (uchar)ui_win_corner_char);

    hook_call(ui_win_frame_on_draw_post, w);

    ui_win_frame_faceify(w, ui_win_frame_face);

    return 0;
}
