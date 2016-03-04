#include <curses.h>
#include <limits.h>
#include <stdlib.h>
#include "err.h"
#include "face.h"
#include "line.h"

/*
 * If I ever meet whatever idiotic arseholes designed ncurses' color system...
 */

#define pairid(fg, bg) (7 - (fg % 8) + (bg % 8) * 8)
#define idgetbg(id)    (id / 8)
#define idgetfg(id)    (7 - id % 8)

void face_initsys(void)
{
    short id;
    id = 0;

    while (++id < 64)
        init_pair(id, idgetfg(id), idgetbg(id));
}

face *face_init(short fgid, short bgid)
{
    face *rtn;

    rtn = malloc(sizeof(face));

    CHECK_ALLOC(face_init, rtn, NULL);

    rtn->fgid = fgid;
    rtn->bgid = bgid;

    return rtn;
}

short face_get_attr(face *f)
{
    return COLOR_PAIR(pairid(f->fgid, f->bgid));
}

int line_add_face(line *l, face *f, colno start, colno end)
{
    size_t numfaces, i;
    face *curr;
    vec  *faces;

    CHECK_NULL_PRM(line_add_face, l, -1);

    faces = line_get_faces(l);

    TRACE_NULL(line_add_face, line_get_faces(l), l, -1);

    numfaces = vec_len(faces);

    if (numfaces == INVALID_INDEX)
    {
        TRACE(line_add_face, vec_len(faces), -1);
        return -1;
    }

    i = 0;
    while (i < numfaces)
    {
        curr = (face *)vec_get(faces, i++);

        TRACE_NULL(line_add_face, vec_get(faces, i++), curr, -1);

        if (curr->end < start)
            break;

        if (curr->start < end)
            TRACE_NONZRO_CALL(line_add_face, vec_del(faces, i--), -1);
    }

    if (f != NULL)
        TRACE_NONZRO_CALL(line_add_face, vec_insert(faces, i, f), -1);

    return 0;
}

