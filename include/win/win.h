#if !defined(WIN_WIN_H)
# define WIN_WIN_H
# include "head.h"

# include "buffer/buffer.h"

typedef struct win_s win;

typedef enum
{
    up,
    down,
    left,
    right
} win_dir;

extern hook win_on_split;
extern hook win_on_create;

extern hook win_on_delete_pre;
extern hook win_on_delete_post;

extern hook win_on_buffer_set;

extern win *win_root;

int win_initsys(void);

int win_killsys(void);

int win_split(win *w, win_dir d);

int win_delete(win *w);

buffer *win_get_buffer(win *w);

int win_set_buffer(win *w, buffer *b);

win *win_get_parent(win *w);

uint win_get_offsetx(win *w);

uint win_get_offsety(win *w);

int win_type_isleaf(win *w);

int win_type_issplitter(win *w);

#endif /* WIN_WIN_H */
