#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "err.h"
#include "ui/util.h"

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
    int maxy, maxx;

    getmaxyx(stdscr, maxy, maxx);

    ASSERT_NCR(move(maxy - 1, 0), critical, return -1);

    if (ui_sbar_content == NULL)
        ASSERT_NCR(clrtoeol(), critical, return -1)
    else
        TRACE_INT(ui_util_draw_text_limited_h(maxx - 1, ui_sbar_content, ' '), return -1);

    return 0;
}

int ui_sbar_set(const char *content)
{
    ui_sbar_content = realloc(ui_sbar_content, strlen(content) + 1);
    strcpy(ui_sbar_content, content);
    ui_sbar_draw();
    refresh();

    return 0;
}
