#ifndef UI_UTIL_H
# define UI_UTIL_H

int ui_util_draw_text_limited_h(int n, const char *text, char filler);

int ui_util_draw_text_limited_v(int n, const char *text, char filler);

int ui_util_clear_area(int x, int y, unsigned int sizex, unsigned int sizey);

#endif /* UI_UTIL_H */
