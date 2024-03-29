#if !defined(WIN_STRUCT_H)
# define WIN_STRUCT_H

# include "win/win.h"
# include "buffer/buffer.h"
# include "container/table.h"

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

struct win_s
{
    table *properties;
    win   *parent;
    union
    {
        struct
        {
            buffer *b;
            char   *sidebar;
            char   *caption;
            ulong   offsetx, offsety;
        } leaf;

        struct
        {
            win       *sub1;
            win       *sub2;
            win_seldir selected;
            uint       sub2offset;
        } split;
    } cont;
    win_contype type;
};

extern win *win_root;

#endif /* WIN_STRUCT_H */
