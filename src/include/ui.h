#ifndef UI_H
# define UI_H
#include "wintree.h"
#include "face.h"

wincont *root;


int ui_initsys(void);

void ui_set_char_defaults(void);

extern char ui_more_left_char;
extern char ui_more_right_char;
extern char ui_window_vertical_char;
extern char ui_window_horizontal_char;
extern char ui_window_corner_char;

extern face *ui_window_border_face;
extern face *ui_window_border_selected_face;

int ui_display_wintrees(void);

#endif /* UI_H */
