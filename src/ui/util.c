#include <curses.h>

#include "err.h"

#include "ui/util.h"

int ui_util_draw_text_limited_h(uint n, const char *text, char filler)
{
    if (text == NULL)
        text = "";

    while (*text)
    {
        if (n < 4 && *(text + 1) && *(text + 2) && *(text + 3))
        {
            do ASSERT_NCR(addch('.'), high, return -1) while (n--);
            return 0;
        }

        ASSERT_NCR(addch((uchar)*(text++)), high, return -1);

        n--;
    }

    while (n--)
        ASSERT_NCR(addch((uchar)filler), high, return -1);

    return 0;
}

int ui_util_draw_text_limited_v(uint n, const char *text, char filler)
{
    int x, curry;

    ASSERT_NCR(x     = getcurx(stdscr), critical, return -1);
    ASSERT_NCR(curry = getcury(stdscr), critical, return -1);

    if (text == NULL)
        text = "";

    while (*text)
    {
        if (n < 2 && *(text + 1))
        {
            do ASSERT_NCR(mvaddch(curry++, x, ':'), high, return -1) while (n--);
            return 0;
        }

        ASSERT_NCR(mvaddch(curry++, x, (uchar)*(text++)), high, return -1);

        n--;
    }

    while (n--)
        ASSERT_NCR(mvaddch(curry++, x, (uchar)filler), high, return -1);

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
