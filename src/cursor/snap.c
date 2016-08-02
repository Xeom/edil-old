#include "ui/win/content.h"
#include "buffer/buffer.h"
#include "win/select.h"
#include "win/size.h"
#include "win/win.h"
#include "err.h"

#include "cursor/snap.h"

static int cursor_snap_in_win_x(colno  x, win *w);
static int cursor_snap_in_win_y(colno  x, win *w);

static ulong cursor_snap_offsetx(win *win, cursor *cur, size_t xpos);
static ulong cursor_snap_offsety(win *win, cursor *cur);

static void cursor_snap_handle_cursor_change_pos(vec *args, hook h);

vec cursor_snap_blacklist;

int cursor_snap_initsys(void)
{
    hook_mount(&cursor_on_change_pos,
               &cursor_snap_handle_cursor_change_pos, 500);

    vec_create(&cursor_snap_blacklist, sizeof(cursor_type *));

    return 0;
}

static int cursor_snap_in_win_x(colno  x, win *w)
{
    return (win_size_get_x(w) - 1 + win_get_offsetx(w)) > x &&
        (win_get_offsetx(w)) <= x;
}

static int cursor_snap_in_win_y(lineno y, win *w)
{
    return (win_size_get_y(w) - 1 + win_get_offsety(w)) > y &&
        (win_get_offsety(w)) <= y;
}

snap_mode cursor_snap_xmode = minimal;
snap_mode cursor_snap_ymode = minimal;

int cursor_snap_y(win *w, cursor *cur)
{
    cursor_type *type;

    type = cursor_get_type(cur);

    if (vec_contains(&cursor_snap_blacklist, &type) ||
        cursor_get_buffer(cur) != win_get_buffer(w))
        return 0;

    win_set_offsety(w, cursor_snap_offsety(w, cur));

    return 0;
}

static ulong cursor_snap_offsety(win *w, cursor *cur)
{
    uint textsize;
    lineno ln;

    textsize = win_size_get_y(w) - 1;
    ln       = cursor_get_ln(cur);

    switch (cursor_snap_ymode)
    {
    case minimal:
        if (ln < textsize / 2 + win_get_offsety(w))
            return ln;
    case bottom: return (textsize     <= ln) ? (ln - textsize + 1) : 0;
    case centre: return (textsize / 2 <  ln) ? (ln - textsize / 2) : 0;
    case top:    return ln;
    default:     return 0;
    }

    return 0;
}

int cursor_snap_x(win *w, cursor *cur, size_t xpos)
{
    cursor_type *type;

    type = cursor_get_type(cur);

    if (vec_contains(&cursor_snap_blacklist, &type) ||
        cursor_get_buffer(cur) != win_get_buffer(w))
        return 0;

    win_set_offsetx(w, cursor_snap_offsetx(w, cur, xpos));

    return 0;
}

static ulong cursor_snap_offsetx(win *w, cursor *cur, size_t xpos)
{
    uint textsize;

    textsize = win_size_get_x(w) - 1;

    if (xpos < textsize)
        return 0;
    switch (cursor_snap_xmode)
    {
    case minimal:
        if (xpos < textsize / 2 + win_get_offsetx(w))
            return xpos;
    case bottom: return (textsize     <= xpos) ? (xpos - textsize + 1) : 0;
    case centre: return (textsize / 2 <  xpos) ? (xpos - textsize / 2) : 0;
    case top:    return xpos;
    default:     return 0;
    }

    return 0;
}

static void cursor_snap_handle_cursor_change_pos(vec *args, hook h)
{
    win    *w;
    cursor *cur;
    buffer *buf;
    size_t xpos;
    lineno curln;

    cur = *(cursor **)vec_item(args, 0);

    w   = win_select_get();
    buf = win_get_buffer(w);

    if (cursor_buffer_selected(buf) != cur)
        return;

    if (!cursor_snap_in_win_y(cursor_get_ln(cur), w))
        cursor_snap_y(w, cur);

    curln = cursor_get_ln(cur);

    if (curln == INVALID_INDEX)
        return;

    xpos = ui_win_content_get_cursor_offset(w, curln);

    if (!cursor_snap_in_win_x(xpos, w))
        cursor_snap_x(w, cur, xpos);
}
