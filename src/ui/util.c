#include <curses.h>

#include "ui/util.h"

int ui_util_draw_text_limited(size_t n, char *text)
{
    while (*text)
    {
        if (n < 4 && *(text + 1) && *(text + 2) && *(text + 3))
        {
            do addch('.'); while (n--);
            return 0;
        }

        addch(*(text++));

        n--;
    }

    while (n--)
        addch(' ');

    return 0;
}
