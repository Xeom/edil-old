#include "wintree.h"
#include "line.h"
#include "err.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <stdio.h>

#define WINTREE_MIN_SIZE 3

#define issplitter(tree) (tree->sub1 && tree->sub2)
#define iscontent(tree) (tree->content)
#define issub1(tree) (tree->parent && (tree->parent->sub1 == tree))
#define issub2(tree) (tree->parent && (tree->parent->sub2 == tree))

struct wintree_s
{
    winsplitdir sdir;
    char        selected;
    size_t      sizex, sizey, relposx, relposy;
    wintree    *parent, *sub1, *sub2;
    wincont    *content;
};

wintree *wintree_root;

hook *wintree_on_resizex;
hook *wintree_on_resizey;
hook *wintree_on_delete;
hook *wintree_on_create;

static void wintree_free_norecurse(wintree *tree);
static void wintree_free(wintree *tree);
static void wintree_move_contents(wintree *dst, wintree *src);

static wintree *wintree_init(wincont *content);
static wintree *wintree_get_sister(wintree *tree);

static int wintree_set_sizex(wintree *tree, size_t newsize);
static int wintree_set_sizey(wintree *tree, size_t newsize);

int wintree_initsys(void)
{
    wintree_on_resizex = hook_init(3);
    wintree_on_resizey = hook_init(3);
    wintree_on_delete = hook_init(1);
    wintree_on_create = hook_init(1);

    wintree_root = wintree_init(wincont_get_root());
    hook_call(wintree_on_create, wintree_root);

    return 0;
}

int wintree_killsys(void)
{
    hook_free(wintree_on_resizex);
    hook_free(wintree_on_resizey);
    hook_free(wintree_on_create);
    hook_free(wintree_on_delete);

    wintree_free(wintree_root);

    return 0;
}

int wintree_set_root_size(size_t x, size_t y)
{
    wintree_set_sizex(wintree_root, x);
    wintree_set_sizey(wintree_root, y);

    return 0;
}

static wintree *wintree_init(wincont *content)
{
    wintree *rtn;

    rtn = malloc(sizeof(wintree));

    CHECK_ALLOC(wintree_init, rtn, NULL);

    /* Easier than setting individual attributes ... */
    memset(rtn, 0, sizeof(wintree));

    rtn->sdir    = none;
    rtn->content = content;
    rtn->selected = 1;

    return rtn;
}

static void wintree_free(wintree *tree)
{
    /* Recurse down the tree if there is one */
    if (issplitter(tree))
    {
        wintree_free(tree->sub1);
        wintree_free(tree->sub2);
    }

    wintree_free_norecurse(tree);
}

/* Used internally for where we need to free a wintree without  *
 * affecting its children                                       */
static void wintree_free_norecurse(wintree *tree)
{
    free(tree);
}

/* Used internally to copy the children etc. of src to dst. The *
 * location and size of dst are preserved, and the children of  *
 * src are adjusted to fit. The contents of src are generally   *
 * mutilated, and should be deleted to avoid bad shit, like     *
 * parents which do not know their child.                       */
static void wintree_move_contents(wintree *dst, wintree *src)
{
    wintree *sub1, *sub2;
    size_t   newx,  newy;

    newx = dst->sizex;
    newy = dst->sizey;

    sub1 = src->sub1;
    sub2 = src->sub2;

    /* Make subs recognise their new parent */
    if (sub1)
        sub1->parent = dst;

    if (sub2)
        sub2->parent = dst;

    /* Copy subs, content, selected, sdir */
    dst->sizex = src->sizex;
    dst->sizey = src->sizey;

    dst->sub1 = sub1;
    dst->sub2 = sub2;

    dst->selected = src->selected;
    dst->sdir     = src->sdir;

    dst->content = src->content;

    wintree_set_sizex(dst, newx);
    wintree_set_sizey(dst, newy);
}

/* Used internally to get the sister of a wintree. i.e. if a    *
 * wintree is sub1 of its parent, this returns sub2 of the      *
 * parent                                                       */
static wintree *wintree_get_sister(wintree *tree)
{
    /* If current tree is the sub1 of its parent, return her *
     * parent's sub2, and vice versa.                        */
    if (issub1(tree))
        return tree->parent->sub2;

    if (issub2(tree))
        return tree->parent->sub1;

    return NULL;
}

int wintree_delete(wintree *tree)
{
    wintree *sister, *parent;

    parent = tree->parent;

    /* No, no you can't delete that */
    if (parent == NULL)
    {
        ERR_NEW(high, "wintree_delete: Tried to delete wintree_root (or wintree with no parent)", NULL);
        return -1;
    }

    sister = wintree_get_sister(tree);

    if (sister == NULL)
    {
        ERR_NEW(critical, "wintree_delete: Could not get sister", "Could not fetch sister to replace parent of tree");
        return -1;
    }
    /* Because the parent splitter  is no longer needed, we can move the sister's *
     * contents into where it used to be.                                         */
    wintree_move_contents(parent, sister);

    wintree_free(tree);
    wintree_free_norecurse(sister);

    return 0;
}

int wintree_swap_prev(wintree *tree)
{
    wincont *prev;

    CHECK_NULL_PRM(wintree_swap_prev, tree, -1);

    prev = wincont_prev(tree->content);
    TRACE_NULL(wintree_swap_prev, wincont_prev(tree->content), prev, -1);

    tree->content = prev;

    return 0;
}

int wintree_swap_next(wintree *tree)
{
    wincont *next;

    CHECK_NULL_PRM(wintree_swap_next, tree, -1);

    next = wincont_next(tree->content);
    TRACE_NULL(wintree_swap_next, wincont_next(tree->content), next, -1);

    tree->content = next;

    return 0;
}

size_t wintree_get_posx(wintree *tree)
{
    size_t pos;

    CHECK_NULL_PRM(wintree_get_posx, tree, INVALID_INDEX);

    pos = 0;
    /* Add up the relative positions of all ancestors */
    while (tree)
    {
        pos += tree->relposx;
        tree = tree->parent;
    }

    return pos;
}

size_t wintree_get_posy(wintree *tree)
{
    size_t pos;

    CHECK_NULL_PRM(wintree_get_posx, tree, INVALID_INDEX);

    pos = 0;
    /* Add up the relative positions of all ancestors */
    while (tree)
    {
        pos += tree->relposy;
        tree = tree->parent;
    }

    return pos;
}

wintree *wintree_get_selected(void)
{
    wintree *tree = wintree_root;

    while (issplitter(tree))
    {

        if      (tree->selected == 0)
                    return tree;
        else if (tree->selected == 1)
            tree = tree->sub1;
        else if (tree->selected == 2)
            tree = tree->sub2;
        else
            return NULL;
    }

    return tree;
}

/* TODO: ERRORS, MAKING THIS SANE */
int wintree_move_border(wintree *tree, int n)
{
    wintree *sub1, *sub2;

    if (!issplitter(tree))
        return 0;

    sub1 = tree->sub1;
    sub2 = tree->sub2;

    if (tree->sdir == lr)
    {
        if (-n >= (int)wintree_get_sizex(sub1) || n >= (int)wintree_get_sizex(sub2))
            return -1;

        wintree_set_sizex(sub1, wintree_get_sizex(sub1) + n);
        wintree_set_sizex(sub2, wintree_get_sizex(sub2) - n);

        sub2->relposx += n;
    }

    if (tree->sdir == ud)
    {
        if (-n >= (int)wintree_get_sizey(sub1) || n >= (int)wintree_get_sizey(sub2))
            return -1;

        wintree_set_sizey(sub1, wintree_get_sizey(sub1) + n);
        wintree_set_sizey(sub2, wintree_get_sizey(sub2) - n);

        sub2->relposy += n;
    }

    return 0;
}

static int wintree_set_sizey(wintree *tree, size_t newsize)
{
    size_t oldsize;
    int delta;

    /* Kill the tree if it's too small *
     * Just like kittens.              */
/*    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);
*/
    oldsize = tree->sizey;
    delta   = newsize - oldsize;

/*    if (oldsize == newsize)
        return 0;
*/
    tree->sizey = newsize;

    /* Content needs no further action */
    if (!issplitter(tree))
        return 0;

    if (tree->sdir == ud)
    {
        /* When the splitter is up/down, we gotta divide up the space */
        wintree_set_sizey(tree->sub1, tree->sub1->sizey + delta / 2);
        tree->sub2->relposy = tree->sub1->sizey;
        wintree_set_sizey(tree->sub2, (newsize - tree->sub1->sizey));
    }

    if (tree->sdir == lr)
    {
        /* When the splitter is left/right, both chlidren get the same y size */
        wintree_set_sizey(tree->sub1, newsize);
        wintree_set_sizey(tree->sub2, newsize);
    }

    return 0;
}

/* Cba writing comments twice. Look up ^^^ */
static int wintree_set_sizex(wintree *tree, size_t newsize)
{
    size_t oldsize;
    int    delta;

/*    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);
*/
    oldsize = tree->sizex;
    delta   = newsize - oldsize;
/*
    if (oldsize == newsize)
        return 0;
*/
    tree->sizex = newsize;

    if (!issplitter(tree))
        return 0;

    if (tree->sdir == lr)
    {
        wintree_set_sizex(tree->sub1, tree->sub1->sizex + delta / 2);
        tree->sub2->relposx = tree->sub1->sizex;
        wintree_set_sizex(tree->sub2, (newsize - tree->sub1->sizex));
    }

    if (tree->sdir == ud)
    {
        wintree_set_sizex(tree->sub1, newsize);
        wintree_set_sizex(tree->sub2, newsize);
    }

    return 0;
}

wintree *wintree_iter_next(wintree *tree)
{
    wintree *parent, *next;
    parent = tree->parent;

    /* If the tree is sub2, continue going up */
    if      (issub2(tree))
        return wintree_iter_next(parent);

    /* If the tree's sub1, we get its sister to be *
     * an ancestor of our return value             */
    else if (issub1(tree))
        next = parent->sub2;

    /* If the tree has no parent, the current tree is *
     * sub2s all the way. This is the last tree.      */
    else
        return NULL;

    /* Decend back down. sub1s all the way. */
    while (next->sub1)
        next = next->sub1;

    return next;
}

wintree *wintree_iter_start(void)
{
    wintree *tree;

    tree = wintree_root;

    /* Keep traversing down sub1s */
    while (tree->sub1)
        tree = tree->sub1;

    return tree;
}

int wintree_select_up(wintree *tree)
{
    wintree *parent;

    parent = tree->parent;

    if (parent == NULL)
    {
        ERR_NEW(high, "wintree_select_up: Tried to go up from wintree root (or root with no parent)", NULL);
        return -1;
    }

    parent->selected = 0;

    return 0;
}

int wintree_select_next(wintree *tree)
{
    /* Notice this just counts the selected positions of *
     * splitters in binary. 111, 112, 121, 122, 211, etc */
    wintree *parent, *next;

    parent = tree->parent;

    /* If the tree is sub2, continue going up */
    if      (issub2(tree))
        return wintree_select_next(parent);

    /* If the tree's sub1, we swap it to sub2 */
    else if (issub1(tree))
    {
        parent->selected = 2;
        next = parent->sub2;
    }
    /* If we got to the root, stop going up. *
     * This makes us loop!                   */
    else
        next = tree;

    /* Decend back down, selecting 1 the whole way */
    while (next)
    {
        next->selected = 1;
        next = next->sub1;
    }

    return 0;
}

int wintree_split(wintree *tree, windir dir)
{
    wintree *sub1, *sub2;
    wincont *newcontent;
    size_t newsize;

    if (iscontent(tree))
        newcontent = wincont_clone(wintree_get_selected()->content);
    else
        newcontent = NULL;

    /* We make two new subs to split into.                     *
     * this seems easier to implement than making a new        *
     * splitter and one new sub, after experimenting.          */
    sub1 = wintree_init(NULL);
    sub2 = wintree_init(NULL);


    /* Move the previous contents into sub2 if we split up or   *
     * left. This means that the new dir will be to the left or *
     * above us, depending which we choose.                     */
    if (dir == up || dir == left)
    {
        wintree_move_contents(sub2, tree);
        sub1->content = newcontent;
        tree->selected = 1;
    }
    else if (dir == down || dir == right)
    {
        wintree_move_contents(sub1, tree);
        sub2->content = newcontent;
        tree->selected = 2;
    }

    if (dir == left || dir == right)
    {
        tree->sdir = lr;

        wintree_set_sizey(sub1, tree->sizey);
        wintree_set_sizey(sub2, tree->sizey);

        /* Divide up the space (relposes are 0 by default) */
        newsize = tree->sizex / 2;
        wintree_set_sizex(sub1, newsize);
        sub2->relposx = newsize;

        newsize = tree->sizex - newsize;
        wintree_set_sizex(sub2, newsize);
    }
    else if (dir == up || dir == down)
    {
        tree->sdir = ud;

        wintree_set_sizex(sub1, tree->sizex);
        wintree_set_sizex(sub2, tree->sizex);

        /* Divide up the space (relposes are 0 by default) */
        newsize = tree->sizey / 2;
        wintree_set_sizey(sub1, newsize);
        sub2->relposy = newsize;

        newsize = tree->sizey - newsize;
        wintree_set_sizey(sub2, newsize);
    }

    tree->content = NULL;

    tree->sub1 = sub1;
    tree->sub2 = sub2;

    sub1->parent = tree;
    sub2->parent = tree;

    /* Since splitting is the only way to make trees, *
     * we put wintree_on_create here                  */
    if (tree->selected == 1)
        hook_call(wintree_on_create, sub1);

    else
        hook_call(wintree_on_create, sub2);

    return 0;
}

/* This is here to annoy Stef(anie) */
#define F_WINTREE_GET(type, name, errrtn)                      \
    type wintree_get_ ## name (wintree *tree)                  \
    {                                                          \
        CHECK_NULL_PRM(wintree_get_ ## name, tree, errrtn);    \
        return tree-> name;                                    \
    }                                                          \

F_WINTREE_GET(size_t,     sizex, INVALID_INDEX)
F_WINTREE_GET(size_t,     sizey, INVALID_INDEX)
F_WINTREE_GET(wincont*, content, NULL)
F_WINTREE_GET(wintree*,  parent, NULL)
