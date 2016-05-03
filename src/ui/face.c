#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "buffer/line.h"
#include "container/vec.h"

#include "ui/face.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * If I ever meet whatever idiotic arseholes designed ncurses' color system  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

const size_t face_serialized_len = 6;

int ui_face_initsys(void)
{
    short id;
    id = 0;

    /* 0 is white on black. We set up colour pairs from 1 to 63 (b on w) */
    while (++id < 64)
        ASSERT_NCR(init_pair(id, pairid_get_fg(id), pairid_get_bg(id)),
                   critical, return -1);

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

int ui_face_draw(face f, uint n)
{
    attr_t attr;
    short  pairid;

    attr   = ui_face_get_attr(f);
    pairid = colours_get_pairid(f.fg, f.bg);

    ASSERT_NCR(chgat((int)n, attr, pairid, NULL),
               high, return -1);

    return 0;
}

int ui_face_draw_at(face f, int x, int y, uint sizex, uint sizey)
{
    attr_t attr;
    short  pairid;

    attr   = ui_face_get_attr(f);
    pairid = colours_get_pairid(f.fg, f.bg);

    while (sizey--)
        ASSERT_NCR(mvchgat(y + (int)sizey, x, (int)sizex, attr, pairid, NULL),
                   high, return -1);

    return 0;
}

/*
 * 0000 1010
 * 1fff 0bbb
 * 1000 00uh
 * 10nn nnnn
 * 10nn nnnn
 * 1000 nnnn
 *
 * f = foreground colour
 * b = background colour
 * u = underline
 * h = bright colour (used for 16 colours)
 * n = length of format
 */
char *ui_face_serialize(face f, short n)
{
    char *rtn;

    rtn = malloc(face_serialized_len + 1);

    rtn[0]  = '\n';

    memset(rtn + 1, 0x80, 5);

    rtn[1] |= (char) f.bg;
    rtn[1] |= (char)(f.fg << 4);

    rtn[2] |= (char) f.bright;
    rtn[2] |= (char)(f.under  << 1);

    rtn[3] |= 0x3f & (char) n;
    rtn[4] |= 0x3f & (char)(n >>  6);
    rtn[5] |= 0x3f & (char)(n >> 12);

    rtn[6] = '\0';

    return rtn;
}

face ui_face_deserialize_face(const char *str)
{
    face rtn;

    memset(&rtn, 0, sizeof(face));

    ASSERT_PTR(str,        high, return rtn);
    ASSERT(str[0] == '\n', high, return rtn);
    ASSERT(str[1] &&
           str[2],         high, return rtn);


    rtn.bg =  str[1] & 0x07;
    rtn.fg = (str[1] & 0x70) >> 4;

    rtn.bright = (char)((str[2] & 0x01));
    rtn.under  = (char)((str[2] & 0x02) >> 1);

    return rtn;
}

short ui_face_deserialize_length(const char *str)
{
    short rtn;
    short bits12;

    bits12 = 0x3f;

    rtn  = 0;
    ASSERT_PTR(str,        high, return rtn);
    ASSERT(str[0] == '\n', high, return rtn);
    ASSERT(str[1] &&
           str[2] &&
           str[3] &&
           str[4] &&
           str[5],         high, return rtn);

    rtn |=  (short) (bits12 & str[3]);
    rtn |=  (short)((bits12 & str[4]) << 6);
    rtn |=  (short)((bits12 & str[5]) << 12);

    return rtn;
}

