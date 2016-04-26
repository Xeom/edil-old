#include <stdlib.h>
#include <string.h>

#include "win/size.h"
#include "win/util.h"
#include "win/label.h"
#include "hook.h"

#include "win/win.h"

win *win_root;

hook_add(win_on_split, 2);

hook_add(win_on_delete_pre,  1);
hook_add(win_on_delete_post, 1);

hook_add(win_on_create, 1);

static win *win_init_leaf(void);
static win *win_init(void);

static void win_free_norecurse(win *w);
static void win_free(win *w);

int win_initsys(void)
{
    win_root = win_init_leaf();
    hook_call(win_on_create, win_root);

    return 0;
}

int win_killsys(void)
{
    hook_call(win_on_delete_pre, win_root);
    win_free(win_root);

    return 0;
}

static win *win_init(void)
{
    win *rtn;

    rtn = malloc(sizeof(win));
    memset(rtn, 0, sizeof(win));

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

static int win_move_contents(win *dst, win *src)
{
    uint sizex, sizey;
    win *par;

    par   = dst->parent;

    sizex = win_size_get_x(dst);
    sizey = win_size_get_y(dst);

    win_size_resize_x(src, sizex);
    win_size_resize_y(src, sizey);

    memcpy(dst, src, sizeof(win));

    dst->parent = par;

    if (win_issplit(src))
    {
        src->cont.split.sub1->parent = dst;
        src->cont.split.sub2->parent = dst;
    }
    else if (win_isleaf(src))
    {
        dst->cont.leaf.caption = NULL;
        dst->cont.leaf.sidebar = NULL;

        win_label_caption_set(dst, win_label_caption_get(src));
        win_label_sidebar_set(dst, win_label_sidebar_get(src));
    }

    if (win_issub1(src))
        src->parent->cont.split.sub1 = dst;

    if (win_issub2(src))
        src->parent->cont.split.sub2 = dst;

    return 0;
}

int win_split(win *w, win_dir d)
{
    win *newleaf, *neww, *nsub1, *nsub2;
    uint sizex, sizey;

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    if (d == up || d == down)
        win_size_resize_y(w, sizey / 2);

    if (d == left || d == right)
        win_size_resize_x(w, sizex / 2);

    neww    = win_init();
    memcpy(neww, w, sizeof(win));

    neww->parent = w;

    if (win_issplit(neww))
    {
        neww->cont.split.sub1->parent = neww;
        neww->cont.split.sub2->parent = neww;
    }

    newleaf = win_init_leaf();
    newleaf->parent = w;

    if (d == up || d == left)
    {
        nsub1 = newleaf;
        nsub2 = neww;
        w->cont.split.selected = sub1;
    }
    else
    {
        nsub1 = neww;
        nsub2 = newleaf;
        w->cont.split.selected = sub2;
    }

    w->cont.split.sub1 = nsub1;
    w->cont.split.sub2 = nsub2;

    if (d == left || d == right)
    {
        w->type = lrsplit;
        w->cont.split.sub2offset = sizex / 2;
    }

    if (d == up || d == down)
    {
        w->type = udsplit;
        w->cont.split.sub2offset = sizey / 2;
    }

    hook_call(win_on_split, w, newleaf);
    hook_call(win_on_create, newleaf);
    hook_call(win_on_create, neww);

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

    else
        return -1;

    hook_call(win_on_delete_pre, w);

    win_move_contents(par, sister);

    win_free(w);
    win_free_norecurse(sister);

    hook_call(win_on_delete_post, par);

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
