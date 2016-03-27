#ifndef UI_FACE_H
# define UI_FACE_H

#include "line.h"

typedef struct face_s face;

#if ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6 ) )
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

int ui_face_initsys(void);

int ui_face_killsys(void);

face *ui_face_init(short fgid, short bgid);

void ui_face_free(face *f);

int ui_face_get_attr(face *f);

int ui_face_add_to_line(face *f, colno start, colno end, line *l);

int ui_face_draw_at(face *f, size_t x, size_t y, size_t sizex, size_t sizey);

#endif /* UI_FACE_H */
