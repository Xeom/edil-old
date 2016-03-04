#include <curses.h>
#include "err.h"
#include "line.h"
#include "ui.h"
#include "wincont.h"
#include "wintree.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

wintree *ui_rootwin;

int ui_display_wintree_border(wintree *tree);
int ui_display_wintree_line(wintree *tree, lineno ln);

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_WHITE);
    ui_set_char_defaults();

    return 0;
}

void ui_set_char_defaults(void)
{
    ui_more_left_char         = '\253';
    ui_more_right_char        = '\273';
    ui_window_vertical_char   = '|';
    ui_window_horizontal_char = '-';
    ui_window_corner_char     = '\'';
}

int ui_display_wintree_line(wintree *tree, lineno ln)
{
    lineno currln;
    size_t currposx, winposx, currposy;
    char *text;

    text = wintree_get_line(tree, ln);
    winposx = wintree_getposx(tree);
    currposy = wintree_getposy(tree);
    currln   = 0;

    while (++currln < wintree_getsizey(tree))
    {
        text = wintree_get_line(tree, currln);
        currposx = winposx;
        while (currposx + 1 < wintree_getsizey(tree))
        {
            if (text)
                mvaddch(currposy, currposx, *(++text));

            else
                mvaddch(currposy, currposx, ' ');

            ++currposx;
        }
        free(text);
        ++currposy;
    }

    return 0;
}

int ui_display_wintree_border(wintree *tree)
{
    size_t posx, posy, lastposx, lastposy;

    posx = wintree_getposx(tree);
    posy = wintree_getposy(tree);

    if (wintree_getsizey(tree) == 0 || wintree_getsizex(tree) == 0)
        return -1;

    lastposy = posy + wintree_getsizey(tree) - 1;
    lastposx = posx + wintree_getsizex(tree) - 1;

    while (posx < lastposx)
    {
        mvaddch(lastposy, posx, ui_window_horizontal_char);
        ++posx;
    }

    while (posy < lastposy)
    {

        mvaddch(posy, lastposx, ui_window_vertical_char);
        ++posy;
    }

    mvaddch(lastposy, lastposx, ui_window_corner_char);

    return 0;
}


int ui_display_wintrees(void)
{
    wintree *selected, *curr;

    selected = wintree_get_selected();
    ui_display_wintree_border(selected);
    curr = wintree_iter_next_content(selected);

    while (curr != selected)
    {
        ui_display_wintree_border(curr);
        curr = wintree_iter_next_content(curr);
    } while (curr != selected);
    return 0;
}
