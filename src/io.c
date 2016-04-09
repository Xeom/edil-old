#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "hook.h"

#include "io.h"

#define  MOD_STR "E-"
#define CTRL_STR "^"
#define  IGN_STR "(Shh)" /* Shh..... */

#define  MOD_STR_LEN (sizeof(MOD_STR)  - 1)
#define CTRL_STR_LEN (sizeof(CTRL_STR) - 1)
#define  IGN_STR_LEN (sizeof(IGN_STR)  - 1)

/* A well chosen and definitely not arbitrary number */
#define KEYNAME_LEN 15

#define CTRL_MODIFIER 1
#define  MOD_MODIFIER 2
#define  IGN_MODIFIER 4

struct io_key_s
{
    char modifiers;
    char keyname[KEYNAME_LEN];
};

io_key io_incomplete_key;

hook_add(io_on_keypress, 1);
hook_add(io_on_ignored_keypress, 1);

/* TODO: Figure if these should be extern */
const int io_key_resize = KEY_RESIZE;
const int io_keyname_len = KEYNAME_LEN;

static void io_clear_incomplete_key(void);
static int  io_handle_completed_key(void);

#define HAS_CTRL(key) ((key.modifiers & CTRL_MODIFIER) != 0)
#define HAS_MOD(key)  ((key.modifiers &  MOD_MODIFIER) != 0)
#define HAS_IGN(key)  ((key.modifiers &  IGN_MODIFIER) != 0)

int io_initsys(void)
{
    io_clear_incomplete_key();

    return 0;
}

int io_killsys(void)
{
    return 0;
}

void io_ignore_curr(void)
{
    io_incomplete_key.modifiers |= IGN_MODIFIER;
}

static void io_clear_incomplete_key(void)
{
    io_incomplete_key.modifiers = 0;
    strcpy(io_incomplete_key.keyname, "<NULL>");
}

char *io_key_str(io_key key)
{
    size_t len;
    char *rtn, *ptr;

    len = (HAS_MOD(key)  ?  MOD_STR_LEN : 0) +
          (HAS_CTRL(key) ? CTRL_STR_LEN : 0) +
          (HAS_IGN(key)  ?  IGN_STR_LEN : 0) +
          KEYNAME_LEN;

    rtn = malloc(len);

    ptr = rtn;

    if (HAS_IGN(key))
    {
        memcpy(ptr, IGN_STR, IGN_STR_LEN);
        ptr += IGN_STR_LEN;
    }

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

    memcpy(ptr, key.keyname, len);

    return rtn;
}

static int io_handle_completed_key(void)
{
    hook_call(io_on_keypress, &io_incomplete_key);

    io_clear_incomplete_key();

    return 0;
}

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
        io_incomplete_key.keyname[0] = (chr + 64) & CHAR_MAX;
        io_incomplete_key.keyname[1] = '\0';
        io_incomplete_key.modifiers |= CTRL_MODIFIER;
    }

    else if (chr < 128) /* Normal ascii character */
    {
        completed                    = 1;
        io_incomplete_key.keyname[0] = chr & CHAR_MAX;
        io_incomplete_key.keyname[1] = '\0';
    }

    else
    {
        const char *name;

        completed = 1;
        name = keyname(chr);

        if (name && strncmp(name, "KEY_", 4) == 0)
        {
            strncpy(io_incomplete_key.keyname, name + 4, KEYNAME_LEN - 1);
            io_incomplete_key.keyname[KEYNAME_LEN - 1] = '\0';
        }
        else
            strcpy(io_incomplete_key.keyname, "<?>");
    }

    if (completed)
        io_handle_completed_key();

    return 0;
}
