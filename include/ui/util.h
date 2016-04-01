#if !defined(UI_UTIL_H)
# define UI_UTIL_H
# include "head.h"

# include "container/vec.h"

int ui_util_draw_vec_limited_v(uint spacelim, char filler, vec *v);

int ui_util_draw_vec_limited_h(uint spacelim, char filler, vec *v);

int ui_util_draw_str_limited_h(uint spacelim, char filler, const char *str);

int ui_util_draw_str_limited_v(uint spacelim, char filler, const char *str);

int ui_util_draw_text_limited_v(uint spacelim, uint strlim, char filler, const char *str);

int ui_util_draw_text_limited_h(uint spacelim, uint strlim, char filler, const char *str);

int ui_util_clear_area(int x, int y, uint sizex, uint sizey);

#endif /* UI_UTIL_H */
