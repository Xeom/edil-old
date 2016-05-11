#if !defined(WIN_LABEL_H)
# define WIN_LABEL_H
# include "head.h"

extern hook win_label_on_caption_set_pre;
extern hook win_label_on_caption_set_post;

extern hook win_label_on_sidebar_set_pre;
extern hook win_label_on_sidebar_set_post;


int win_label_caption_set(win *w, const char *caption);

int win_label_sidebar_set(win *w, const char *caption);

const char *win_label_caption_get(win *w);

const char *win_label_sidebar_get(win *w);

#endif /* WIN_LABEL_H */
