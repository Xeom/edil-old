#include <stdlib.h>

#include "win/size.h"
#include "win/util.h"
#include "win/label.h"

#include "win/win.h"

win *win_root;

static win *win_init_leaf(void);
static win *win_init(void);

static void win_free_norecurse(win *w);
static void win_free(win *w);

int win_initsys(void)
{
    win_root = win_init_leaf();

    return 0;
}

int win_killsys(void)
{
    win_free(win_root);

    return 0;
}

static win *win_init(void)
{
    win *rtn;

    rtn = malloc(sizeof(win));

    return rtn;
}

static win *win_init_leaf(void)
{
    win *rtn;

    rtn = win_init();

    rtn->type = leaf;
    rtn->cont.leaf.b = buffer_init();

    win_label_sidebar_set(rtn, "");
    win_label_caption_set(rtn, "");

    rtn->cont.leaf.offsetx = 0;
    rtn->cont.leaf.offsety = 0;

    return rtn;
}

static void win_free_norecurse(win *w)
{
    if (win_isleaf(w))
    {
        free(w->cont.leaf.sidebar);
        free(w->cont.leaf.caption);
    }

    free(w);
}

static void win_free(win *w)
{
    if (win_issplit(w))
    {
        win_free(w->cont.split.sub1);
        win_free(w->cont.split.sub2);
    }

    win_free_norecurse(w);
}


int win_split(win *w, win_dir d)
{
    win *split, *newleaf, *sub1, *sub2;
    uint sizex, sizey;

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    split   = win_init();
    newleaf = win_init_leaf();

    if (d == up || d == left)
    {
        sub1 = newleaf;
        sub2 = w;
    }

    if (d == down || d == right)
    {
        sub1 = w;
        sub2 = newleaf;
    }

    if (d == left || d == right)
    {
        split->type = lrsplit;
        split->cont.split.sub2offset = sizex / 2;

        win_size_resize_x(sub1, sizex / 2);
        win_size_resize_x(sub2, sizex - sizex / 2);
    }

    if (d == up || d == down)
    {
        split->cont.split.sub2offset = sizey / 2;
        split->type = udsplit;

        win_size_resize_y(sub1, sizey / 2);
        win_size_resize_y(sub2, sizey - sizey / 2);
    }

    split->cont.split.sub1 = sub1;
    split->cont.split.sub2 = sub2;

    sub1->parent = split;
    sub2->parent = split;

    return 0;
}

int win_delete(win *w)
{
    win *sister, *par;

    if (win_isroot(w))
        return 0;

    par = w->parent;

    if (win_issub1(w))
        sister = par->cont.split.sub2;

    if (win_issub2(w))
        sister = par->cont.split.sub1;

    if (par->type == lrsplit)
        win_size_resize_x(sister, win_size_get_x(par));

    if (par->type == udsplit)
        win_size_resize_y(sister, win_size_get_y(par));

    sister->parent = par->parent;

    if (win_issub1(par))
        par->parent->cont.split.sub1 = sister;

    if (win_issub2(par))
        par->parent->cont.split.sub2 = sister;

    win_free(w);
    win_free_norecurse(par);

    return 0;
}

win *win_get_parent(win *w)
{
    if (win_isroot(w))
        return NULL;

    return w->parent;
}

buffer *win_get_buffer(win *w)
{
    if (!win_isleaf(w))
        return NULL;

    return w->cont.leaf.b;
}

uint win_get_offsetx(win *w)
{
    if (!win_isleaf(w))
        return 0;

    return w->cont.leaf.offsetx;
}

uint win_get_offsety(win *w)
{
    if (!win_isleaf(w))
        return 0;

    return w->cont.leaf.offsety;
}
