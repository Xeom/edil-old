#include <curses.h>
#include <string.h>

#include "err.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE char
#define VEC_TYPED_NAME char
#include "container/typed_vec.h"

#include "ui/util.h"

int ui_util_draw_vec_limited_h(uint spacelim, char filler, vec *v)
{
    char *iter;
    iter = vec_char_item((vec_char *)v, 0);

    ui_util_draw_text_limited_h(spacelim, (uint)vec_char_len((vec_char *)v), filler, iter);

    return 0;
}

int ui_util_draw_vec_limited_v(uint spacelim, char filler, vec *v)
{
    char *iter;

iter = vec_char_item((vec_char *)v, 0);

    return ui_util_draw_text_limited_v(spacelim, (uint)vec_char_len((vec_char *)v), filler, iter);
}

int ui_util_draw_str_limited_h(uint spacelim, char filler, const char *str)
{
    size_t len;
    len = strlen(str);

    ui_util_draw_text_limited_h(spacelim, (uint)len, filler, str);

    return 0;
}

int ui_util_draw_str_limited_v(uint spacelim, char filler, const char *str)
{
    size_t len;
    len = strlen(str);

    ui_util_draw_text_limited_v(spacelim, (uint)len, filler, str);

    return 0;
}

int ui_util_draw_text_limited_h(uint spacelim, uint strlim, char filler, const char *str)
{
    while (spacelim && strlim--)
    {
        spacelim--;
        addch((uchar)*(str++));
    }

    while (spacelim--)
        addch((uchar)filler);

    return 0;
}

int ui_util_draw_text_limited_v(uint spacelim, uint strlim, char filler, const char *str)
{
    int currx, curry;

    currx = getcurx(stdscr);
    curry = getcury(stdscr);

    while (spacelim && strlim--)
    {
        spacelim--;
        mvaddch(curry++, currx, (uchar)*(str++));
    }

    while (spacelim--)
        mvaddch(curry++, currx, (uchar)filler);

    return 0;
}

int ui_util_clear_area(int x, int y, uint sizex, uint sizey)
{
    int currx, curry;
    int  maxx,  maxy;

    maxx = x + (int)sizex;
    maxy = y + (int)sizey;

    for (currx = x; currx < maxx; currx++)
        for (curry = y; curry < maxy; curry++)
            mvaddch(curry, currx, ' ');

    return 0;
}
