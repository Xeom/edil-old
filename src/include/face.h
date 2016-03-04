#ifndef FACE_H
# define FACE_H
#include "line.h"

typedef struct face_s face;

struct face_s
{
    short bgid, fgid;
    colno start;
    colno   end;
};

void face_initsys(void);

face *face_init(short fgid, short bgid);

short face_get_attr(face *f);

int line_add_face(line *l, face *f, colno start, colno end);

#endif /* FACE_H */
