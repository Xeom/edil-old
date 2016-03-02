#ifndef UI_H
# define UI_H
#include "wintree.h"

wincont *root;

int ui_initsys(void);

void ui_set_char_defaults(void);

char ui_more_left_char;
char ui_more_right_char;
char ui_window_vertical_char;
char ui_window_horizontal_char;
char ui_window_corner_char;

int ui_display_wintree(wintree *tree);

#endif /* UI_H */
