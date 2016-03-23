#include <curses.h>
#include <string.h>
#include <stdlib.h>

#include "hook.h"

#include "io.h"

#define  MOD_STR "E-"
#define CTRL_STR "^"

#define  MOD_STR_LEN (sizeof(MOD_STR)  - 1)
#define CTRL_STR_LEN (sizeof(CTRL_STR) - 1)

#define KEYNAME_LEN 4

#define CTRL_MODIFIER 1
#define  MOD_MODIFIER 2

struct io_key_s
{
    char modifiers;
    char keyname[KEYNAME_LEN];
};

io_key io_incomplete_key;

hook_add(io_on_keypress, 1);

const int io_key_resize = KEY_RESIZE;

static void io_clear_incomplete_key(void);
static int io_handle_completed_key(void);

#define HAS_CTRL(key) ((key.modifiers & CTRL_MODIFIER) != 0)
#define HAS_MOD(key)  ((key.modifiers &  MOD_MODIFIER) != 0)

int io_initsys(void)
{
    io_clear_incomplete_key();

    return 0;
}

int io_killsys(void)
{
    hook_free(io_on_keypress);

    return 0;
}

static void io_clear_incomplete_key(void)
{
    io_incomplete_key.modifiers = 0;
    strcpy(io_incomplete_key.keyname, "<?>");
}

char *io_key_str(io_key key)
{
    char *rtn, *ptr;

    rtn = malloc((HAS_MOD(key)  ? MOD_STR_LEN : 0) +
                 (HAS_CTRL(key) ? CTRL_STR_LEN : 0) +
                 KEYNAME_LEN);

    ptr = rtn;

    if (HAS_MOD(key))
    {
        memcpy(ptr, MOD_STR, MOD_STR_LEN);
        ptr += MOD_STR_LEN;
    }

    if (HAS_CTRL(key))
    {
        memcpy(ptr, CTRL_STR, CTRL_STR_LEN);
        ptr += CTRL_STR_LEN;
    }

    memcpy(ptr, key.keyname, KEYNAME_LEN);

    return rtn;
}

static int io_handle_completed_key(void)
{
    hook_call(io_on_keypress, &io_incomplete_key);

    io_clear_incomplete_key();

    return 0;
}

#include <stdio.h>
int io_handle_chr(int chr)
{
    int completed;

    if (chr == 27)
    {
        completed                    = 0;
        io_incomplete_key.modifiers |= MOD_MODIFIER;
    }
    else if (chr < 32) /* ASCII Ctrl-Key control char */
    {
        completed                    = 1;
        io_incomplete_key.keyname[0] = chr + 64;
        io_incomplete_key.keyname[1] = '\0';
        io_incomplete_key.modifiers |= CTRL_MODIFIER;
    }

    else if (chr < 128) /* Normal ascii character */
    {
        completed                    = 1;
        io_incomplete_key.keyname[0] = chr;
        io_incomplete_key.keyname[1] = '\0';
    }

    else
    {
        /* Dunno LOL */
    }

    if (completed)
        io_handle_completed_key();

    return 0;
}
