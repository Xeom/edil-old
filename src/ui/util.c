#include <curses.h>
#include <string.h>

#include "err.h"

#define VEC_TYPED_GETSET
#define VEC_TYPED_TYPE char
#define VEC_TYPED_NAME char
#include "container/typed_vec.h"

#include "ui/face.h"

#include "ui/util.h"


int ui_util_draw_vec_limited_h(uint spacelim, char filler, vec *v)
{
    char *iter;
    iter = vec_char_item((vec_char *)v, 0);

    ui_util_draw_text_limited_h(spacelim, (uint)vec_char_len((vec_char *)v),
                                filler, iter);

    return 0;
}

int ui_util_draw_vec_limited_v(uint spacelim, char filler, vec *v)
{
    char *iter;

iter = vec_char_item((vec_char *)v, 0);

    ui_util_draw_text_limited_v(spacelim, (uint)vec_char_len((vec_char *)v),
                                filler, iter);

    return 0;
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

int ui_util_draw_text_limited_h(
    uint spacelim, uint strlim, char filler, const char *str)
{
    face f;
    int n;
    n = 0;
    while (spacelim && strlim--)
    {
        uchar chr;
        chr = (uchar)*(str);

        if (chr == '\n')
        {
            f = ui_face_deserialize_face(str);
            n = ui_face_deserialize_length(str);

            str    += 5;
            strlim -= 5;
        }
        else
        {
            if (n)
            {
                n--;
                addch((chtype)chr | ui_face_get_attr(f));
            }
            else
                addch(chr);
        }

        str++;
        spacelim--;
    }

    while (spacelim--)
    {
        if (n)
        {
            n--;
            addch((chtype)filler | ui_face_get_attr(f));
        }
        else
            addch((chtype)filler);
    }

    return 0;
}

int ui_util_draw_text_limited_v(
    uint spacelim, uint strlim, char filler, const char *str)
{
    int currx, curry;

    currx = getcurx(stdscr);
    curry = getcury(stdscr);

    ui_util_clear_area(currx, curry, 1, spacelim);

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
