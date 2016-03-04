#include <curses.h>
#include "include/wintree.h"
#include "include/ui.h"
int main(void)
{
    size_t termx, termy;

    wincont_initsys();
    wintree_initsys();
    ui_initsys();

    getmaxyx(stdscr, termy, termx);
    wintree_set_root_size(termx, termy );
    wintree_split(wintree_get_selected(), left);
    wintree_split(wintree_get_selected(), up);
    wintree_split(wintree_get_selected(), right);
    wintree_split(wintree_get_selected(), down);
    ui_display_wintrees();
    refresh();
    printf("%hu\n",can_change_color());
    printf("Pairs: %hu\n", COLOR_PAIRS);
    sleep(100);
    endwin();
    return 0;
}
