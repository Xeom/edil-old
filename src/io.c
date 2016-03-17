#include <string.h>
#include <stdlib.h>
#include "io.h"

#define KEYNAME_LEN 4

struct io_key_s
{
    unsigned int mod  :1;
    unsigned int ctrl :1;
    char         keyname[KEYNAME_LEN];
};

io_key incomplete_key;

#define MOD_STR  "M-"
#define CTRL_STR "C-"

#define  MOD_STR_LEN (sizeof(MOD_STR)  - 1)
#define CTRL_STR_LEN (sizeof(CTRL_STR) - 1)

char *io_key_str(io_key key)
{
    char *rtn, *ptr;

    rtn = malloc((key.mod  ? MOD_STR_LEN : 0) +
                 (key.ctrl ? CTRL_STR_LEN : 0) +
                 KEYNAME_LEN);

    ptr = rtn;

    if (key.mod)
    {
        memcpy(ptr, MOD_STR, MOD_STR_LEN);
        ptr += MOD_STR_LEN;
    }

    if (key.ctrl)
    {
        memcpy(ptr, CTRL_STR, CTRL_STR_LEN);
        ptr += CTRL_STR_LEN;
    }

    memcpy(ptr, key.keyname, KEYNAME_LEN);

    return rtn;
}

int io_handle_completed_key(io_key key);

int io_handle_chr(int chr)
{
    int completed;

    if (chr == 27)
    {
        completed          = 0;
        incomplete_key.mod = 1;
    }
    else if (chr < 32) /* ASCII Ctrl-Key control char */
    {
        completed                 = 1;
        incomplete_key.keyname[0] = chr + 64;
        incomplete_key.keyname[1] = '\0';
        incomplete_key.ctrl       = 1;
    }

    else if (chr < 128) /* Normal ascii character */
    {
        completed                 = 1;
        incomplete_key.keyname[0] = chr;
        incomplete_key.keyname[1] = '\0';
    }

    else
    {
        /* Dunno LOL */
    }

    return 0;
}
