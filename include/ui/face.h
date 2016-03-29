#ifndef UI_FACE_H
# define UI_FACE_H
# include "head.h"

# include "buffer/line.h"

typedef struct face_s face;

struct face_s
{
    uint bgid   :3;
    uint fgid   :3;
    uint bright :1;
    uint under  :1;

    colno start;
    colno   end;
};

int ui_face_initsys(void);

int ui_face_killsys(void);

face *ui_face_init(ushort fgid, ushort bgid);

void ui_face_free(face *f);

int ui_face_get_attr(face *f);

int ui_face_add_to_line(face *f, colno start, colno end, line *l);

int ui_face_draw_at(face *f, int x, int y, uint sizex, uint sizey);

#endif /* UI_FACE_H */
