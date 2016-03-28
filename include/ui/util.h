#ifndef UI_UTIL_H
# define UI_UTIL_H
# include "head.h"

int ui_util_draw_text_limited_h(uint n, const char *text, char filler);

int ui_util_draw_text_limited_v(uint n, const char *text, char filler);

int ui_util_clear_area(int x, int y, uint sizex, uint sizey);

#endif /* UI_UTIL_H */
