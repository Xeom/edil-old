#if !defined(WIN_LABEL_H)
# define WIN_LABEL_H
# include "head.h"

int win_label_caption_set(win *w, const char *caption);

int win_label_sidebar_set(win *w, const char *caption);

const char *win_label_caption_get(win *w);

const char *win_label_sidebar_get(win *w);

#endif /* WIN_LABEL_H */
