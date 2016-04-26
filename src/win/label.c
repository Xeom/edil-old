#include <stdlib.h>
#include <string.h>

#include "win/util.h"

#include "win/label.h"

static const char *blank = "";

hook_add(win_label_on_caption_set_pre,  2);
hook_add(win_label_on_caption_set_post, 2);

hook_add(win_label_on_sidebar_set_pre,  2);
hook_add(win_label_on_sidebar_set_post, 2);

int win_label_caption_set(win *w, const char *caption)
{
    size_t len;

    if (!win_isleaf(w))
        return 0;

//    hook_call(win_label_on_caption_set_pre, w, &caption);

    len = strlen(caption) + 1;

    if (w->cont.leaf.caption)
        free(w->cont.leaf.caption);

    w->cont.leaf.caption = malloc(len);

    strcpy(w->cont.leaf.caption, caption);

    hook_call(win_label_on_caption_set_post, w, &caption);

    return 0;
}

int win_label_sidebar_set(win *w, const char *sidebar)
{
    size_t len;

    if (!win_isleaf(w))
        return 0;

//    hook_call(win_label_on_sidebar_set_pre, w, &sidebar);

    len = strlen(sidebar) + 1;

    if (w->cont.leaf.sidebar)
        free(w->cont.leaf.sidebar);

    w->cont.leaf.sidebar = malloc(len);

    strcpy(w->cont.leaf.sidebar, sidebar);

//    hook_call(win_label_on_sidebar_set_post, w, &sidebar);

    return 0;
}

const char *win_label_caption_get(win *w)
{
    if (!win_isleaf(w) ||
        !w->cont.leaf.caption)
        return blank;

    return w->cont.leaf.caption;
}

const char *win_label_sidebar_get(win *w)
{
    if (!win_isleaf(w) ||
        !w->cont.leaf.sidebar)
        return blank;

    return w->cont.leaf.sidebar;
}

