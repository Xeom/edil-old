#include <unistd.h>

#include "ui/core.h"
#include "win/win.h"
#include "win/select.h"
#include "win/iter.h"
#include "err.h"
#include "io.h"

main ()
{
    err_initsys();
    win_initsys();
    ui_initsys();
    io_initsys();
    ui_resize();

    win_split(win_root, up);
    win_split(win_select_get(), left);
    ui_refresh();

    io_killsys();
    ui_killsys();

    win *iter, *last;

    iter = win_iter_first(win_root);
    last = win_iter_last(win_root);
    while (iter != last)
    {
        buffer_free(win_get_buffer(iter));
        iter = win_iter_next(iter);
    }
    buffer_free(win_get_buffer(iter));

    win_killsys();
    err_killsys();

    return 0;
}
