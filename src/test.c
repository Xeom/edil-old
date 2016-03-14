#include <curses.h>
#include <signal.h>
#include "wintree.h"
#include "ui.h"
#include "err.h"

int main(void)
{
    size_t termx, termy;

    err_initsys();
    wincont_initsys();
    wintree_initsys();
    ui_initsys();
    face_initsys();

    ui_resize();
    wintree_split(wintree_get_selected(), left);
    wintree_split(wintree_get_selected(), up);
    wintree_split(wintree_get_selected(), right);
    wintree_split(wintree_get_selected(), down);

    puts("DONE SPLITTING");
    while (1)
    {
        wintree_select_next(wintree_get_selected());
        ui_display_wintrees();
/*        refresh();*/
        getch();
    }

    endwin();
    return 0;
}
