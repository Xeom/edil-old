#include <stdlib.h>
#include <string.h>

#include "win/size.h"
#include "win/util.h"
#include "win/label.h"
#include "win/select.h"
#include "hook.h"
#include "err.h"

#include "win/win.h"

win *win_root;

/* Hook called when a window is split with the win_split function.            *
 *                                                                            *
 * Called with two arguments - A pointer to the window that was split and the *
 * newly created leaf window. The second window is a child of the first       */
hook_add(win_on_split, 2);

/* Hook called before a window is about to be deleted and free'd.             *
 *                                                                            *
 * Called with one argument - A pointer to the window about to be deleted.    */
hook_add(win_on_delete_pre,  1);

/* Hook called after a window is deleted and free'd.                          *
 *                                                                            *
 * Called with one argument - The parent of the deleted window.               */
hook_add(win_on_delete_post, 1);

/* Hook called whenever a new window is created by win_split. Called for both *
 * the new leaf and parent window. (Note that it's not called for the sibling *
 * of the newly created leaf window.)                                         *
 *                                                                            *
 * Called with one argument - A pointer to the new window.                    */
hook_add(win_on_create, 1);

/* Hook called after the buffer associated with a window is set.              *
 *                                                                            *
 * Called with two arguments - A pointer to the window and to the old buffer. */
hook_add(win_on_buffer_set, 2);

/* Hook called after the offset x or y of a window is set.                    *
 *                                                                            *
 * Called with two arguments - A pointer to the window and to the old offset. */
hook_add(win_on_offsetx_set, 2);
hook_add(win_on_offsety_set, 2);

/*
 * Initialize a window, and set it up with the default parameters of a leaf 
 * window, with a new, blank buffer in it.
 *
 * @return A pointer to the new window.
 *
 */
static win *win_init_leaf(void);

/*
 * Initialize a new window, with all parameters not set.
 *
 * @return A pointer to the new window.
 *
 */
static win *win_init(void);

/*
 * Free a window, its sidebar and caption.
 *
 * @param w A pointer to the window to free.
 *
 */
static void win_free_norecurse(win *w);

/*
 * Free a window, its children and all their sidebars and captions.
 *
 *
 * @param w A pointer to the window to free.
 *
 */
static void win_free(win *w);

/*
 * Transfer the contents of one window into another. The sidebar, parent, etc.
 * of the source window are transfered to the destination window. If the source
 * window is a splitter, its children are resized appropriately and their parent
 * is set to the destination window. After all this, the source window becomes
 * messed up - its children won't recognise it anymore.
 *
 * @param dst A pointer to the destination window.
 * @param src A pointer to the source window.
 *
 * @return 0 on success, -1 on failure.
 *
 */
static int win_move_contents(win *dst, win *src);

int win_initsys(void)
{
    /* Initialize a root window. All windows will be children of this window. */
    win_root = win_init_leaf();

    hook_call(win_on_create, win_root);

    return 0;
}

int win_killsys(void)
{
    /* Delete the root window */
    hook_call(win_on_delete_pre, win_root);
    win_free(win_root);

    /* The root window has no parent, so we need to call *
     * win_on_delete_post with NULL.                     */
    hook_call(win_on_delete_post, NULL);

    return 0;
}

static win *win_init(void)
{
    win *rtn;

    /* Allocate and initialize a new struct */
    rtn = malloc(sizeof(win));
    memset(rtn, 0, sizeof(win));

    return rtn;
}

static win *win_init_leaf(void)
{
    win *rtn;

    /* Get a new window */
    rtn = win_init();

    /* Set the type to leaf */
    rtn->type = leaf;

    /* Give the new window an empty buffer */
    rtn->cont.leaf.b = buffer_init();

    /* Give the window empty sidebars and captions */
    win_label_sidebar_set(rtn, "");
    win_label_caption_set(rtn, "");

    /* Set its offsets to nothing */
    rtn->cont.leaf.offsetx = 0;
    rtn->cont.leaf.offsety = 0;

    return rtn;
}

static void win_free_norecurse(win *w)
{
    /* If the window is a leaf, free its caption and sidebar text */
    if (win_isleaf(w))
    {
        free(w->cont.leaf.sidebar);
        free(w->cont.leaf.caption);
    }

    /* Free the window itself */
    free(w);
}

static void win_free(win *w)
{
    /* If the window is a splitter, recurse to its subs */
    if (win_issplit(w))
    {
        win_free(w->cont.split.sub1);
        win_free(w->cont.split.sub2);
    }

    /* Free the window itself */
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

    /* Copy any other random stuff we left laying around. */
    memcpy(dst, src, sizeof(win));

    /* Give the destination its new parent */
    dst->parent = par;

    if (win_issplit(src))
    {
        /* Fix up the children's pointers */
        src->cont.split.sub1->parent = dst;
        src->cont.split.sub2->parent = dst;
    }
    else if (win_isleaf(src))
    {
        /* Copy the caption and sidebar so they don't get free'd with *
         * the source window.                                         */
        dst->cont.leaf.caption = NULL;
        dst->cont.leaf.sidebar = NULL;

        win_label_caption_set(dst, win_label_caption_get(src));
        win_label_sidebar_set(dst, win_label_sidebar_get(src));
    }

    /* Make sure the parent of the source window is updated to point to the *
     * destination window.                                                  */
    if (win_issub1(src))
        src->parent->cont.split.sub1 = dst;

    if (win_issub2(src))
        src->parent->cont.split.sub2 = dst;

    return 0;
}

win *win_split(win *w, win_dir d)
{
    win *newleaf, *neww, *nsub1, *nsub2;
    uint sizex, sizey;

    /* Get the original size of the window to be split. */
    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    /* Resize the window to half its original size *
     * in an appropriate direction.                */
    if (d == up || d == down)
        win_size_resize_y(w, sizey / 2);

    if (d == left || d == right)
        win_size_resize_x(w, sizex / 2);

    neww = win_init();
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

    return newleaf;
}

int win_delete(win *w)
{
    win *sister, *par;

    if (win_isroot(w))
        return 0;

    par = w->parent;

    if      (win_issub1(w))
        sister = par->cont.split.sub2;

    else if (win_issub2(w))
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

int win_set_buffer(win *w, buffer *b)
{
    buffer *old;

    if (!win_isleaf(w))
        return -1;

    if (!b)
        return -1;

    old = w->cont.leaf.b;

    w->cont.leaf.b = b;

    hook_call(win_on_buffer_set, w, old);

    return 0;
}


ulong win_get_offsetx(win *w)
{
    if (!win_isleaf(w))
        return 0;

    return w->cont.leaf.offsetx;
}

ulong win_get_offsety(win *w)
{
    if (!win_isleaf(w))
        return 0;

    return w->cont.leaf.offsety;
}

int win_set_offsetx(win *w, ulong new)
{
    ulong old;

    ASSERT(win_isleaf(w), high, return -1);

    old = w->cont.leaf.offsetx;

    if (old == new)
        return 0;

    w->cont.leaf.offsetx = new;

    hook_call(win_on_offsetx_set, w, &old);

    return 0;
}

int win_set_offsety(win *w, ulong new)
{
    ulong old;

    ASSERT(win_isleaf(w), high, return -1);
    if (new > 1000)
        return *((int *)NULL);
    old = w->cont.leaf.offsety;

    if (old == new)
        return 0;

    w->cont.leaf.offsety = new;

    hook_call(win_on_offsety_set, w, &old);

    return 0;
}

int win_type_isleaf(win *w)
{
    return win_isleaf(w);
}

int win_type_issplitter(win *w)
{
    return win_issplit(w);
}
