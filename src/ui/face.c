#include <curses.h>
#include <limits.h>
#include <stdlib.h>

#include "err.h"
#include "line.h"

#include "ui/face.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * If I ever meet whatever idiotic arseholes designed ncurses' color system... *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This translates an fg and bg into the pairid it should be allocated 
 * It's a bit odd since id 0 must be white fg, black bg
 */
#define pairid(fg, bg) (7 - (fg % 8) + (bg % 8) * 8)

/* These macros just get the original fg and bg from a pairid */
#define idgetbg(id)    (id / 8)
#define idgetfg(id)    (7 - id % 8)

/* These get attributes and pairids from a face * */
#define f_getattr(f)   ( (f->under ? A_UNDERLINE : 0) | (f->bright ? A_BOLD : 0) )
#define f_getpairid(f) ( pairid(f->fgid, f->bgid) )

void ui_face_initsys(void)
{
    short id;
    id = 0;

    /* 0 is white on black. We set up colour pairs from 1 to 63 (black on white) */
    while (++id < 64)
        init_pair(id, idgetfg(id), idgetbg(id));
}

int ui_face_killsys(void)
{
    return 0;
}

face *ui_face_init(short fgid, short bgid)
{
    face *rtn;

    rtn = malloc(sizeof(face));

    CHECK_ALLOC(face_init, rtn, NULL);

    rtn->fgid = fgid;
    rtn->bgid = bgid;

    rtn->under = 0;
    rtn->bright = 0;

    return rtn;
}

int ui_face_get_attr(face *f)
{
    CHECK_NULL_PRM(face_get_attr, f, 0);

    return f_getattr(f) | COLOR_PAIR(f_getpairid(f));
}

int ui_face_add_to_line(face *f, colno start, colno end, line *l)
{
    vec  *faces;

    CHECK_NULL_PRM(line_add_face, l, -1);

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

    return 0;
}

int ui_face_draw_at(face *f, size_t x, size_t y, size_t sizex, size_t sizey)
{
    attr_t attr;
    int    colorid;

    attr    = f_getattr(f);
    colorid = f_getpairid(f);

    while (sizey--)
        mvchgat(y + sizey, x, sizex, attr, colorid, NULL);

    return 0;
}