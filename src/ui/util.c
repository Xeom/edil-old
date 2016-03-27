#include <curses.h>

#include "err.h"

#include "ui/util.h"

int ui_util_draw_text_limited_h(int n, const char *text, char filler)
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

        ASSERT_NCR(addch(*(text++)), high, return -1);

        n--;
    }

    while (n--)
        ASSERT_NCR(addch(filler), high, return -1);

    return 0;
}

int ui_util_draw_text_limited_v(int n, const char *text, char filler)
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

        ASSERT_NCR(mvaddch(curry++, x, *(text++)), high, return -1);

        n--;
    }

    while (n--)
        ASSERT_NCR(mvaddch(curry++, x, filler), high, return -1);

    return 0;
}

int ui_util_clear_area(int x, int y, unsigned int sizex, unsigned int sizey)
{
    int currx, curry;
    int  maxx,  maxy;

    maxx = x + sizex;
    maxy = y + sizey;

    for (currx = x; currx < maxx; currx++)
        for (curry = y; curry < maxy; curry++)
            mvaddch(curry, currx, ' ');

    return 0;
}
