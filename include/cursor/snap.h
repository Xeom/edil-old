#if !defined(CURSOR_SNAP_H)
# define CURSOR_SNAP_H
# include "head.h"

# include "cursor/cursor.h"

typedef enum
{
    centre,
    top,
    bottom,
    minimal
} snap_mode;

extern snap_mode cursor_snap_xmode;
extern snap_mode cursor_snap_ymode;

int cursor_snap_initsys(void);

int cursor_snap_y(win *w, cursor *cur);

int cursor_snap_x(win *w, cursor *cur, size_t xpos);

#endif /* CURSOR_SNAP_H */
