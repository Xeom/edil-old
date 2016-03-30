#include <curses.h>
#include <limits.h>
#include <stdlib.h>

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
#define pairid(fg, bg) (short)(7 - (fg % 8) + (bg % 8) * 8)

/* These macros just get the original fg and bg from a pairid */
#define idgetbg(id)    (short)(id / 8)
#define idgetfg(id)    (short)(7 - id % 8)

/* These get attributes and pairids from a face * */
#define f_getattr(f)   (attr_t)( (f->under ? A_UNDERLINE : 0) | (f->bright ? A_BOLD : 0) )
#define f_getpairid(f) ( pairid(f->fgid, f->bgid) )

int ui_face_initsys(void)
{
    short id;
    id = 0;

    /* 0 is white on black. We set up colour pairs from 1 to 63 (black on white) */
    while (++id < 64)
        ASSERT_NCR(init_pair(id, idgetfg(id), idgetbg(id)) != OK, critical, return -1);

    return 0;
}

int ui_face_killsys(void)
{
    return 0;
}

face *ui_face_init(ushort fgid, ushort bgid)
{
    face *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(face)), terminal, return NULL);

    rtn->fgid = (uint)(fgid & 7);
    rtn->bgid = (uint)(bgid & 7);

    rtn->under = 0;
    rtn->bright = 0;

    return rtn;
}

void ui_face_free(face *f)
{
    free(f);
}

int ui_face_get_attr(face *f)
{
    attr_t attr, color;

    ASSERT_PTR(f, high, return 0);

    attr  = f_getattr(f);
    color = COLOR_PAIR(f_getpairid(f));

    return (int)(attr | color);
}

int ui_face_add_to_line(face *f, colno start, colno end, line *l)
{
    vec  *faces;
/*
    ASSERT_PTR(f, high, return -1);

    TRACE_PTR(faces = line_get_faces(l), return -1);

    vec_rforeach(faces, face *, curr,
                 if (curr->start < end)
                     TRACE_INT(vec_delete(faces, _vec_index, 1),
                               return -1);

                 if (curr->end < start)
                 {
                     TRACE_INT(vec_insert(faces, _vec_index + 1, 1, f),
                               return -1);
                     return 0;
                 }
        );
*/
    return 0;
}

int ui_face_draw_at(face *f, int x, int y, uint sizex, uint sizey)
{
    attr_t attr;
    short  colorid;

    ASSERT_PTR(f, high, return -1);

    attr    = f_getattr(f);
    colorid = f_getpairid(f);

    while (sizey--)
        ASSERT_NCR(mvchgat(y + (int)sizey, x, (int)sizex, attr, colorid, NULL), high, return -1);

    return 0;
}
