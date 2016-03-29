#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "err.h"
#include "ui/win.h"

#include "wintree.h"

#define WINTREE_MIN_SIZE 3

struct wintree_s
{
    char        *caption, *sidebar;

    char        selected;
    winsplitdir sdir;
    uint        sizex, sizey;
    int         relposx, relposy;

    wintree    *parent, *sub1, *sub2;
    wincont    *content;
};

wintree *wintree_root;

hook_add(wintree_on_resizex, 3);
hook_add(wintree_on_resizey, 3);
hook_add(wintree_on_delete,  1);
hook_add(wintree_on_create,  1);
hook_add(wintree_on_split,   1);

static void wintree_free_norecurse(wintree *tree);
static void wintree_free(wintree *tree);

static int wintree_move_contents(wintree *dst, wintree *src);

static wintree *wintree_init(wincont *content);
static wintree *wintree_get_sister(wintree *tree);

static int wintree_set_sizex(wintree *tree, uint newsize);
static int wintree_set_sizey(wintree *tree, uint newsize);

#define issplitter(tree) (tree->sub1 && tree->sub2)
#define iscontent(tree) (tree->content)
#define issub1(tree) (tree->parent && (tree->parent->sub1 == tree))
#define issub2(tree) (tree->parent && (tree->parent->sub2 == tree))

int wintree_initsys(void)
{
    ASSERT_PTR(wintree_root = wintree_init(wincont_get_root()), terminal,
               return -1);

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

static wintree *wintree_init(wincont *content)
{
    wintree *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(wintree)), terminal,
               return NULL);

    /* Easier than setting individual attributes ... */
    memset(rtn, 0, sizeof(wintree));

    rtn->caption = NULL;
    rtn->sidebar = NULL;
    rtn->sdir    = none;
    rtn->content = content;
    rtn->selected = 1;

    return rtn;
}

/* Used internally for where we need to free a wintree without  *
 * affecting its children                                       */
static void wintree_free_norecurse(wintree *tree)
{
    free(tree);
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

int wintree_set_root_size(uint x, uint y)
{
    TRACE_INT(wintree_set_sizex(wintree_root, x),
              return -1);
    TRACE_INT(wintree_set_sizey(wintree_root, y),
              return -1);

    TRACE_INT(ui_win_draw(),
              return -1);

    return 0;
}

int wintree_set_caption(wintree *tree, const char *caption)
{
    ASSERT_PTR(tree->caption = realloc(tree->caption, strlen(caption)),
               terminal, return -1);

    strcpy(tree->caption, caption);

    TRACE_INT(ui_win_draw_subframes(tree),
              return -1);

    return 0;
}

int wintree_set_sidebar(wintree *tree, const char *sidebar)
{
    ASSERT_PTR(tree->sidebar = realloc(tree->sidebar, strlen(sidebar)),
               terminal, return -1);

    strcpy(tree->sidebar, sidebar);

    TRACE_INT(ui_win_draw_subframes(tree),
              return -1);

    return 0;
}

/* Used internally to copy the children etc. of src to dst. The *
 * location and size of dst are preserved, and the children of  *
 * src are adjusted to fit. The contents of src are generally   *
 * mutilated, and should be deleted to avoid bad shit, like     *
 * parents which do not know their child.                       */
static int wintree_move_contents(wintree *dst, wintree *src)
{
    wintree *sub1, *sub2;
    uint     newx,  newy;

    ASSERT_PTR(dst, critical, return -1);
    ASSERT_PTR(src, critical, return -1);

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

    ASSERT_INT(wintree_set_sizex(dst, newx), critical,
               return -1);
    ASSERT_INT(wintree_set_sizey(dst, newy), critical,
               return -1);

    return 0;
}

/* Used internally to get the sister of a wintree. i.e. if a    *
 * wintree is sub1 of its parent, this returns sub2 of the      *
 * parent                                                       */
static wintree *wintree_get_sister(wintree *tree)
{
    ASSERT_PTR(tree, critical, return NULL);

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

    ASSERT_PTR(tree, high, return -1);

    parent = tree->parent;

    /* No, no you can't delete that */
    ASSERT_PTR(parent, high, return -1);

    sister = wintree_get_sister(tree);

    ASSERT_PTR(sister, critical, return -1);

    /* Because the parent splitter  is no longer needed, we can move the sister's *
     * contents into where it used to be.                                         */
    TRACE_INT(wintree_move_contents(parent, sister), return -1);

    wintree_free(tree);
    wintree_free_norecurse(sister);

    TRACE_INT(ui_win_draw_sub(parent), return -1);

    return 0;
}

int wintree_swap_prev(wintree *tree)
{
    wincont *prev;

    ASSERT_PTR(tree, high, return -1);

    TRACE_PTR(prev = wincont_prev(tree->content),
              return -1);

    tree->content = prev;

    TRACE_INT(ui_win_draw_sub(tree),
              return -1);

    return 0;
}

int wintree_swap_next(wintree *tree)
{
    wincont *next;

    ASSERT_PTR(tree, high, return -1);

    ASSERT_PTR(next = wincont_next(tree->content), high,
               return -1);

    tree->content = next;

    TRACE_INT(ui_win_draw_sub(tree),
              return -1);

    return 0;
}

int wintree_get_posx(wintree *tree)
{
    int pos;

    ASSERT_PTR(tree, high, return -1);

    pos = 0;
    /* Add up the relative positions of all ancestors */
    while (tree)
    {
        pos += tree->relposx;
        tree = tree->parent;
    }

    return pos;
}

int wintree_get_posy(wintree *tree)
{
    int pos;

    ASSERT_PTR(tree, high, return -1);

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

        wintree_set_sizex(sub1, (uint)((int)wintree_get_sizex(sub1) + n));
        wintree_set_sizex(sub2, (uint)((int)wintree_get_sizex(sub2) - n));

        sub2->relposx += n;
    }

    if (tree->sdir == ud)
    {
        if (-n >= (int)wintree_get_sizey(sub1) || n >= (int)wintree_get_sizey(sub2))
            return -1;

        wintree_set_sizey(sub1, (uint)((int)wintree_get_sizey(sub1) + n));
        wintree_set_sizey(sub2, (uint)((int)wintree_get_sizey(sub2) - n));

        sub2->relposy += n;
    }

    ui_win_draw_sub(tree);

    return 0;
}

static int wintree_set_sizey(wintree *tree, uint newsize)
{
    uint oldsize;
    int  delta;

    hook_call(wintree_on_resizey, &tree, &(tree->sizex), &newsize);
    /* Kill the tree if it's too small *
     * Just like kittens.              */
/*    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);
*/
    oldsize = tree->sizey;
    delta   = (int)(newsize - oldsize);

    if (oldsize == newsize)
        return 0;

    tree->sizey = newsize;

    /* Content needs no further action */
    if (!issplitter(tree))
        return 0;

    if (tree->sdir == ud)
    {
        /* When the splitter is up/down, we gotta divide up the space */
        wintree_set_sizey(tree->sub1, (uint)((int)tree->sub1->sizey + delta / 2));
        tree->sub2->relposy = (int)tree->sub1->sizey;
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
static int wintree_set_sizex(wintree *tree, uint newsize)
{
    uint oldsize;
    int  delta;

    hook_call(wintree_on_resizex, &tree, &newsize, &(tree->sizey));
/*    if (newsize < WINTREE_MIN_SIZE)
        wintree_delete(tree);
*/
    oldsize = tree->sizex;
    delta   = (int)(newsize - oldsize);

    if (oldsize == newsize)
        return 0;

    tree->sizex = newsize;

    if (!issplitter(tree))
        return 0;

    if (tree->sdir == lr)
    {
        wintree_set_sizex(tree->sub1, (uint)((int)tree->sub1->sizex + delta / 2));
        tree->sub2->relposx = (int)tree->sub1->sizex;
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

wintree *wintree_iter_subs_start(wintree *tree)
{
    /* Keep traversing down sub1s */
    while (tree->sub1)
        tree = tree->sub1;

    return tree;
}

wintree *wintree_iter_subs_end(wintree *tree)
{
    /* Keep traversing down sub2s */
    while (tree->sub2)
        tree = tree->sub2;

    return tree;
}

wintree *wintree_iter_start(void)
{
    return wintree_iter_subs_start(wintree_root);
}

int wintree_select(wintree *tree)
{
    ASSERT_PTR(tree, high, return -1);

    TRACE_INT(ui_win_draw_highlight(ui_win_frame_face),
              return -1);

    tree->selected = 0;

    while (tree->parent)
    {
        if      (issub1(tree))
            tree->parent->selected = 1;
        else if (issub2(tree))
            tree->parent->selected = 2;

        tree =  tree->parent;
    }

    TRACE_INT(ui_win_draw_highlight(ui_win_frame_sel_face),
              return -1);

    return 0;
}

int wintree_split(wintree *tree, windir dir)
{
    wintree *sub1, *sub2;
    wincont *newcontent;
    uint     newsize;

    if (iscontent(tree))
        newcontent = wincont_clone(wintree_get_selected()->content);
    else
        newcontent = NULL;

    /* We make two new subs to split into.                     *
     * this seems easier to implement than making a new        *
     * splitter and one new sub, after experimenting.          */
    TRACE_PTR(sub1 = wintree_init(NULL), return -1);
    TRACE_PTR(sub2 = wintree_init(NULL), return -1);

    /* Move the previous contents into sub2 if we split up or   *
     * left. This means that the new dir will be to the left or *
     * above us, depending which we choose.                     */
    if (dir == up || dir == left)
    {
        TRACE_INT(wintree_move_contents(sub2, tree), return -1);
        sub1->content = newcontent;
        tree->selected = 1;
    }
    else if (dir == down || dir == right)
    {
        TRACE_INT(wintree_move_contents(sub1, tree), return -1);
        sub2->content = newcontent;
        tree->selected = 2;
    }

    if (dir == left || dir == right)
    {
        tree->sdir = lr;

        TRACE_INT(wintree_set_sizey(sub1, tree->sizey), return -1);
        TRACE_INT(wintree_set_sizey(sub2, tree->sizey), return -1);

        /* Divide up the space (relposes are 0 by default) */
        newsize = tree->sizex / 2;
        TRACE_INT(wintree_set_sizex(sub1, newsize), return -1);
        sub2->relposx = (int)newsize;

        newsize = tree->sizex - newsize;
        TRACE_INT(wintree_set_sizex(sub2, newsize), return -1);
    }
    else if (dir == up || dir == down)
    {
        tree->sdir = ud;

        TRACE_INT(wintree_set_sizex(sub1, tree->sizex), return -1);
        TRACE_INT(wintree_set_sizex(sub2, tree->sizex), return -1);

        /* Divide up the space (relposes are 0 by default) */
        newsize = tree->sizey / 2;
        TRACE_INT(wintree_set_sizey(sub1, newsize), return -1);
        sub2->relposy = (int)newsize;

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

    TRACE_INT(ui_win_draw_sub(tree), return -1);

    return 0;
}

static char *blank = "";

char *wintree_get_sidebar(wintree *tree)
{
    ASSERT_PTR(tree, high, return blank);

    if (tree->sidebar)
        return tree->sidebar;

    return blank;
}

char *wintree_get_caption(wintree *tree)
{
    ASSERT_PTR(tree, high, return blank);

    if (tree->caption)
        return tree->caption;

    return blank;
}

uint wintree_get_sizex(wintree *tree)
{
    ASSERT_PTR(tree, high, return 0);
    return tree->sizex;
}

uint wintree_get_sizey(wintree *tree)
{
    ASSERT_PTR(tree, high, return 0);
    return tree->sizey;
}

wincont *wintree_get_content(wintree *tree)
{
    ASSERT_PTR(tree, high, return 0);
    return tree->content;
}

wintree *wintree_get_parent(wintree *tree)
{
    ASSERT_PTR(tree, high, return 0);
    return tree->parent;
}
