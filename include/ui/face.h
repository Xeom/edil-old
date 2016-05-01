#if !defined(UI_FACE_H)
# define UI_FACE_H
# include "head.h"

# include <curses.h>

# include "buffer/line.h"

typedef enum
{
    black    = COLOR_BLACK,
    blue     = COLOR_BLUE,
    green    = COLOR_GREEN,
    cyan     = COLOR_CYAN,
    red      = COLOR_RED,
    magenta  = COLOR_MAGENTA,
    yellow   = COLOR_YELLOW,
    white    = COLOR_WHITE
} colour;

typedef struct face_s face;

struct face_s
{
    colour bg;
    colour fg;
    uint bright :1;
    uint under  :1;
};

extern const colour face_colour_black;
extern const colour face_colour_blue;
extern const colour face_colour_green;
extern const colour face_colour_cyan;
extern const colour face_colour_red;
extern const colour face_colour_magenta;
extern const colour face_colour_yellow;
extern const colour face_colour_white;

int ui_face_initsys(void);

int ui_face_killsys(void);

face ui_face_init(void);

attr_t ui_face_get_attr(face f);

int ui_face_draw_at(face f, int x, int y, uint sizex, uint sizey);

char *ui_face_serialize(face f, short n);

face ui_face_deserialize_face(char *str);

short ui_face_deserialize_length(char *str);

#endif /* UI_FACE_H */
