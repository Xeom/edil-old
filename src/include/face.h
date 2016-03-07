#ifndef FACE_H
# define FACE_H
#include "line.h"

typedef struct face_s face;

#if ( __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
# define GCC_HAS_DIAGNOSTIC
#endif

struct face_s
{
#ifdef GCC_HAS_DIAGNOSTIC
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"
# define BITFIELD_TYPE unsigned char
#else
# define BITFIELD_TYPE unsigned int
#endif
    BITFIELD_TYPE bgid   :3;
    BITFIELD_TYPE fgid   :3;
    BITFIELD_TYPE bright :1;
    BITFIELD_TYPE under  :1;
#ifdef GCC_HAS_DIAGNOSTIC
# pragma GCC diagnostic pop
# undef GCC_HAS_DIAGNOSTIC
#endif
    colno start;
    colno   end;
};
#undef BITFIELD_TYPE

void face_initsys(void);

face *face_init(short fgid, short bgid);

int face_get_attr(face *f);

int line_add_face(line *l, face *f, colno start, colno end);

#endif /* FACE_H */
