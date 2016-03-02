#include "wintree.h"
#include "line.h"
#include "err.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#define issplitter(tree) (tree->sub1 && tree->sub2)
#define iscontent(tree) (tree->content)

struct wintree_s
{
    winsplitdir sdir;
    windir      selected;
    size_t      sizex, sizey, relposx, relposy;
    wintree    *parent, *sub1, *sub2;
    wincont    *content;
};

#define WINTREE_MIN_SIZE 3

wintree *wintree_root;

int wintree_initsys(void)
{
    wintree_root = wintree_init(wincont_get_root());

    return 0;
}

int wintree_set_root_size(size_t x, size_t y)
{
    wintree_root->sizex = x;
    wintree_root->sizey = y;
    return 0;
}

wintree *wintree_init(wincont *content)
{
    wintree *rtn;

    rtn = malloc(sizeof(wintree));
    memset(rtn, 0, sizeof(wintree));
    rtn->sdir = none;
    rtn->content = content;

    return rtn;
}

void wintree_free_norecurse(wintree *tree)
{
    free(tree);
}

void wintree_free(wintree *tree)
{
    if (issplitter(tree))
    {
        wintree_free(tree->sub1);
        wintree_free(tree->sub2);
    }

    wintree_free_norecurse(tree);
}

wintree *wintree_delete_sub1(wintree *tree)
{
    wintree *sub1, *sub2;

    sub1 = tree->sub1;
    sub2 = tree->sub2;

    wintree_setsizex(sub2, wintree_getsizex(tree));
    wintree_setsizey(sub2, wintree_getsizey(tree));

    sub2->parent = tree->parent;

    if (tree->parent)
    {
        if (tree == tree->parent->sub1)
            tree->parent->sub1 = sub2;

        if (tree == tree->parent->sub2)
            tree->parent->sub1 = sub2;
    }

    wintree_free(sub1);

    wintree_free_norecurse(tree);

    return 0;
}

wintree *wintree_delete_sub2(wintree *tree)
{
    wintree *sub1, *sub2;

    sub1 = tree->sub1;
    sub2 = tree->sub2;

    wintree_setsizex(sub1, wintree_getsizex(tree));
    wintree_setsizey(sub1, wintree_getsizey(tree));

    sub2->parent = tree->parent;

    if (tree->parent)
    {
        if (tree == tree->parent->sub1)
            tree->parent->sub1 = sub1;

        if (tree == tree->parent->sub2)
            tree->parent->sub1 = sub1;
    }

    wintree_free(sub2);

    wintree_free_norecurse(tree);

    return 0;
}

int wintree_delete(wintree *tree)
{
    wintree *parent;

    parent = tree->parent;

    if (parent)
    {
        if (parent->sub1 == tree)
            wintree_delete_sub1(parent);

        else if (parent->sub2 == tree)
            wintree_delete_sub2(parent);
    }

    return 0;
}

int wintree_swap_prev(wintree *tree)
{
    wincont *prev;

    CHECK_NULL_PRM(wintree_swap_prev, tree, -1);

    prev = wincont_prev(tree->content);
    TRACE_NULL(wintree_swap_prev, wincont_prev(tree->content), prev, -1);

    return 0;
}

int wintree_swap_next(wintree *tree)
{
    wincont *next;

    CHECK_NULL_PRM(wintree_swap_next, tree, -1);

    next = wincont_next(tree->content);
    TRACE_NULL(wintree_swap_next, wincont_next(tree->content), next, -1);

    return 0;
}

size_t wintree_getposx(wintree *tree)
{
    size_t pos;

    pos = 0;

    while (tree)
    {
        pos += tree->relposx;
        tree = tree->parent;
    }

    return pos;
}

size_t wintree_getposy(wintree *tree)
{
    size_t pos;

    pos = 0;

    while (tree)
    {
        pos += tree->relposy;
        tree = tree->parent;
    }

    return pos;
}

wintree *wintree_get_sub_selected(wintree *tree)
{
    if (issplitter(tree))
    {
        if (tree->selected == up || tree->selected == left)
            return wintree_get_sub_selected(tree->sub1);

        else if (tree->selected == down || tree->selected == right)
            return wintree_get_sub_selected(tree->sub2);
    }

    return tree;
}

wintree *wintree_get_selected(void)
{
    return wintree_get_sub_selected(wintree_root);
}

int wintree_move_border(wintree *tree, int n)
{
    wintree *sub1, *sub2;

    if (!issplitter(tree))
        return 0;

    sub1 = tree->sub1;
    sub2 = tree->sub2;

    if (tree->sdir == lr)
    {
        if (-n >= (int)wintree_getsizex(sub1) || n >= (int)wintree_getsizex(sub2))
            return -1;

        wintree_setsizex(sub1, wintree_getsizex(sub1) + n);
        wintree_setsizex(sub2, wintree_getsizex(sub2) - n);

        sub2->relposx += n;
    }


    if (tree->sdir == ud)
    {
        if (-n >= (int)wintree_getsizey(sub1) || n >= (int)wintree_getsizey(sub2))
            return -1;

        wintree_setsizey(sub1, wintree_getsizey(sub1) + n);
        wintree_setsizey(sub2, wintree_getsizey(sub2) - n);

        sub2->relposy += n;
    }

    return 0;
}

size_t wintree_getsizex(wintree *tree)
{
    return tree->sizex;
}

size_t wintree_getsizey(wintree *tree)
{
    return tree->sizey;
}

int wintree_setsizey(wintree *tree, size_t newsize)
{
    size_t oldsize;

    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);

    oldsize = wintree_getsizey(tree);

    if (oldsize == newsize)
        return 0;

    tree->sizey = newsize;

    if (!issplitter(tree))
        return 0;

    if (tree->sdir == ud)
    {
        wintree_setsizey(tree->sub1, (newsize - oldsize) / 2);
        wintree_setsizey(tree->sub2, (newsize - tree->sub1->sizex));
    }

    if (tree->sdir == lr)
    {
        wintree_setsizey(tree->sub1, newsize);
        wintree_setsizey(tree->sub2, newsize);
    }

    return 0;
}

int wintree_setsizex(wintree *tree, size_t newsize)
{
    size_t oldsize;

    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);

    oldsize = tree->sizex;

    if (oldsize == newsize)
        return -1;

    tree->sizex = newsize;

    if (!issplitter(tree))
        return 0;

    if (tree->sdir == lr)
    {
        wintree_setsizex(tree->sub1, (newsize - oldsize) / 2);
        wintree_setsizex(tree->sub2, (newsize - tree->sub1->sizex));
    }

    if (tree->sdir == ud)
    {
        wintree_setsizex(tree->sub1, newsize);
        wintree_setsizex(tree->sub2, newsize);
    }

    return 0;
}

wintree *wintree_iter_next_content(wintree *tree)
{
    wintree *parent, *next;
    parent = tree->parent;

    if (parent)
    {
        if (tree == parent->sub1)
            next = parent->sub2;
        else
            next = wintree_iter_next_content(parent);
    }
    else
    {
        next = tree;
    }

    while (next->sub1)
    {
        next = next->sub1;
    }

    return next;
}

int wintree_split(wintree *tree, windir dir)
{
    wintree *splitter, *sub1, *sub2;
    size_t newsize;

    splitter = wintree_init(NULL);

    splitter->selected = dir;

    splitter->sizex   = tree->sizex;
    splitter->sizey   = tree->sizey;
    splitter->relposx = tree->relposx;
    splitter->relposy = tree->relposy;

    if (dir == up || dir == left)
    {
        sub1 = wintree_init(tree->content);
        sub2 = tree;
    }
    else
    {
        sub1 = tree;
        sub2 = wintree_init(tree->content);
    }

    splitter->sub1 = sub1;
    splitter->sub2 = sub2;

    sub1->relposx = 0;
    sub1->relposy = 0;

    if (dir == left || dir == right)
    {
        splitter->sdir = lr;
        splitter->selected = left;

        newsize = splitter->sizex / 2;
        wintree_setsizex(sub1, newsize);

        newsize = splitter->sizex - newsize;
        wintree_setsizex(sub2, newsize);

        wintree_setsizey(sub1, wintree_getsizey(splitter));
        wintree_setsizey(sub2, wintree_getsizey(splitter));

        sub2->relposx = sub1->sizex;
        sub2->relposy = 0;
    }

    else if (dir == up || dir == down)
    {
        splitter->sdir = ud;
        splitter->selected = up;

        newsize = splitter->sizey / 2;
        wintree_setsizey(sub1, newsize);

        newsize = splitter->sizey - newsize;
        wintree_setsizey(sub2, newsize);

        wintree_setsizex(sub1, splitter->sizex);
        wintree_setsizex(sub2, splitter->sizex);

        sub2->relposx = 0;
        sub2->relposy = sub1->sizey;
    }

    splitter->parent = tree->parent;

    if (splitter->parent)
    {
        if (splitter->parent->sub1 == tree)
            splitter->parent->sub1  = splitter;
        else
            splitter->parent->sub2  = splitter;
    }

    sub1->parent = splitter;
    sub2->parent = splitter;

    return 0;
}

char *wintree_get_line(wintree *tree, lineno ln)
{
    char *rtn;
    line *l;
    wincont *content;

    CHECK_NULL_PRM(ui_display_wintree_line, tree,  NULL);

    if (ln == 0)
        return NULL;
    
    if (ln >= tree->sizey)
        return NULL;

    content = tree->content;
    l = wincont_get_line(content, ln - 1);

    TRACE_NULL(wintree_get_line, wincont_get_line(content, ln), l, NULL);

    rtn = line_get_text(l);

    TRACE_NULL(wintree_get_line, line_get_text(l), rtn, NULL);

    return rtn;
}
