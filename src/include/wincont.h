#ifndef WINCONT_H
# define WINCONT_H
#include "line.h"

typedef struct wincont_s wincont;

int wincont_initsys(void);

wincont *wincont_init(textcont *text);

wincont *wincont_clone(wincont *cont);

wincont *wincont_next(wincont *cont);

wincont *wincont_prev(wincont *cont);

wincont *wincont_get_root(void);

line *wincont_get_line(wincont *cont, lineno ln);

#endif /* WINCONT_H */
