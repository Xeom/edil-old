#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "ui/sbar.h"

char *ui_sbar_content;

int ui_sbar_initsys(void)
{
    ui_sbar_content = NULL;

    return 0;
}

int ui_sbar_killsys(void)
{
    free(ui_sbar_content);

    return 0;
}

int ui_sbar_draw(void)
{
    char  *content;
    size_t maxx, maxy;

    getmaxyx(stdscr, maxy, maxx);
    move(maxy - 1, 0);

    content = ui_sbar_content;

    while (maxx-- > 4)
    {
        addch(*content);
        ++content;

        if (*content == '\0')
        {
            clrtoeol();
            return 0;
        }
    }

    while (maxx--)
        addch('.');

    return 0;
}

int ui_sbar_set(const char *content)
{
    ui_sbar_content = realloc(ui_sbar_content, strlen(content));
    strcpy(ui_sbar_content, content);
    ui_sbar_draw();
    refresh();

    return 0;
}
