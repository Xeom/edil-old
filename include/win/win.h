#if !defined(WIN_WIN_H)
# define WIN_WIN_H
# include "head.h"

typedef struct win_s win;

typedef enum
{
    lrsplit,
    udsplit,
    leaf
} win_contype;

typedef enum
{
    sub1,
    sub2,
    none
} win_seldir;

typedef enum
{
    up,
    down,
    left,
    right
} win_dir;

int win_initsys(void);

int win_killsys(void);

int win_split(win *w, win_dir d);

int win_delete(win *w);

#endif /* WIN_WIN_H */
