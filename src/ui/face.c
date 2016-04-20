#include "err.h"
#include "buffer/line.h"
#include "container/vec.h"

#include "ui/face.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * If I ever meet whatever idiotic arseholes designed ncurses' color system... *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This translates an fg and bg into the pairid it should be allocated 
 * It's a bit odd since id 0 must be white fg, black bg
 */
#define colours_get_pairid(fg, bg) (short)(7 - (fg % 8) + (bg % 8) * 8)

/* These macros just get the original fg and bg from a pairid */
#define pairid_get_bg(id) (short)(id / 8)
#define pairid_get_fg(id) (short)(7 - id % 8)


const colour face_colour_black   = black;
const colour face_colour_blue    = blue;
const colour face_colour_green   = green;
const colour face_colour_cyan    = cyan;
const colour face_colour_red     = red;
const colour face_colour_magenta = magenta;
const colour face_colour_yellow  = yellow;
const colour face_colour_white   = white;

int ui_face_initsys(void)
{
    short id;
    id = 0;

    /* 0 is white on black. We set up colour pairs from 1 to 63 (black on white) */
    while (++id < 64)
        ASSERT_NCR(init_pair(id, pairid_get_fg(id), pairid_get_bg(id)), critical, return -1);

    return 0;
}

int ui_face_killsys(void)
{
    return 0;
}

attr_t ui_face_get_attr(face f)
{
    attr_t rtn;

    rtn = 0;

    if (f.under)
        rtn |= A_UNDERLINE;

    if (f.bright)
        rtn |= A_BOLD;

    rtn |= COLOR_PAIR(
        colours_get_pairid(f.fg, f.bg));

    return rtn;
}

face ui_face_init(void)
{
    face rtn;

    rtn.bg = black;
    rtn.fg = white;

    rtn.bright = 0;
    rtn.under  = 0;

    return rtn;
}

int ui_face_draw_at(face f, int x, int y, uint sizex, uint sizey)
{
    attr_t attr;
    short  pairid;

    attr   = ui_face_get_attr(f);
    pairid = colours_get_pairid(f.fg, f.bg);

    while (sizey--)
        ASSERT_NCR(mvchgat(y + (int)sizey, x, (int)sizex, attr, pairid, NULL), high, return -1);

    return 0;
}
