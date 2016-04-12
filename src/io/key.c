#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "hook.h"

#include "io/key.h"

#define ESC_STR "E-"
#define CON_STR "^"
#define IGN_STR "(Shh)" /* Shh..... */

#define ESC_STR_LEN (sizeof(ESC_STR)  - 1)
#define CON_STR_LEN (sizeof(CON_STR) - 1)
#define IGN_STR_LEN (sizeof(IGN_STR)  - 1)

/* A well chosen and definitely not arbitrary number */

char io_key_con_mod = 1;
char io_key_esc_mod = 2;
char io_key_ign_mod = 4;

key io_key_incomplete;

hook_add(io_key_on_key,      1);

const int io_key_resize   = KEY_RESIZE;
const int io_key_name_len = IO_KEY_NAME_LEN;

static void io_key_clear(void);
static int  io_key_handle_completed(void);

#define HAS_CON(key) ((key.modifiers & io_key_con_mod) != 0)
#define HAS_ESC(key) ((key.modifiers & io_key_esc_mod) != 0)
#define HAS_IGN(key) ((key.modifiers & io_key_ign_mod) != 0)

key io_key_set_name(key k, const char *str)
{
    size_t final_index;

    final_index = IO_KEY_NAME_LEN - 1;

    k.keyname[final_index] = '\0';
    strncpy(k.keyname, str, final_index);

    return k;
}

int io_key_initsys(void)
{
    io_key_clear();

    return 0;
}

int io_key_killsys(void)
{
    return 0;
}

static void io_key_clear(void)
{
    io_key_incomplete.modifiers = 0;
    strcpy(io_key_incomplete.keyname, "<NULL>");
}

char *io_key_str(key k)
{
    size_t len;
    char *rtn, *ptr;

    len = (HAS_ESC(k) ? ESC_STR_LEN : 0) +
          (HAS_CON(k) ? CON_STR_LEN : 0) +
          (HAS_IGN(k) ? IGN_STR_LEN : 0) +
          IO_KEY_NAME_LEN;

    rtn = malloc(len);

    ptr = rtn;

    if (HAS_IGN(k))
    {
        memcpy(ptr, IGN_STR, IGN_STR_LEN);
        ptr += IGN_STR_LEN;
    }

    if (HAS_ESC(k))
    {
        memcpy(ptr, ESC_STR, ESC_STR_LEN);
        ptr += ESC_STR_LEN;
    }

    if (HAS_CON(k))
    {
        memcpy(ptr, CON_STR, CON_STR_LEN);
        ptr += CON_STR_LEN;
    }

    memcpy(ptr, k.keyname, IO_KEY_NAME_LEN);

    return rtn;
}

static int io_key_handle_completed(void)
{
    key *complete;

    complete = &io_key_incomplete;

    hook_call(io_key_on_key, complete);

    io_key_clear();

    return 0;
}

int io_key_handle_chr(int chr)
{
    if (chr == 27)
    {
        io_key_incomplete.modifiers |= io_key_esc_mod;
        return 0;
    }

    else if (chr < 32) /* ASCII Ctrl-Key control char */
    {
        char name[2];
        name[0] = (chr + 64) & CHAR_MAX;
        name[1] = '\0';

        io_key_incomplete = io_key_set_name(io_key_incomplete, name);
        io_key_incomplete.modifiers |= io_key_con_mod;
    }

    else if (chr < 128) /* Normal ascii character */
    {
        char name[2];
        name[0] = chr & CHAR_MAX;
        name[1] = '\0';

        io_key_incomplete = io_key_set_name(io_key_incomplete, name);
    }

    else
    {
        const char *name;

        name = keyname(chr);

        if (name && strncmp(name, "KEY_", 4) == 0)
            io_key_incomplete = io_key_set_name(io_key_incomplete, name + 4);
        else
            io_key_incomplete = io_key_set_name(io_key_incomplete, "<?>");
    }

    io_key_handle_completed();

    return 0;
}
