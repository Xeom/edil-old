#include "hook.h"
#include "err.h"
#include "win/win.h"
#include "ui/ui.h"
#include "io/key.h"

#include "core.h"

int initsys(void)
{
    win_initsys();
    ui_initsys();
    io_key_initsys();

    return 0;
}
