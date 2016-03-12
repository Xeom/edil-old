#ifndef UI_H
# define UI_H
#include "wintree.h"
#include "face.h"

int ui_initsys(void);

extern volatile char ui_more_left_char;
extern volatile char ui_more_right_char;
extern volatile char ui_window_vertical_char;
extern volatile char ui_window_horizontal_char;
extern volatile char ui_window_corner_char;

extern face *ui_window_border_face;
extern face *ui_window_border_selected_face;

extern const int ui_key_resize;
int ui_display_wintrees(void);

int ui_killsys(void);

int ui_resize(void);

#endif /* UI_H */
