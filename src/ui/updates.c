#include <curses.h>

#include "hook.h"
#include "buffer/buffer.h"
#include "buffer/point.h"

#include "win/win.h"
#include "win/label.h"
#include "win/select.h"
#include "win/size.h"
#include "win/iter.h"

#include "ui/ui.h"
#include "ui/win/win.h"
#include "ui/win/frame.h"
#include "ui/win/content.h"

#include "cursor/cursor.h"

#include "container/vec.h"

#include "ui/updates.h"


static buffer *ui_updates_holding_buffer = NULL;

#define unpack_arg(num, type, name)             \
    {                                           \
        if (vec_item(args, num) == NULL)        \
            return;                             \
        name = *(type **)vec_item(args, num);   \
    }

static void ui_updates_win_label_set  (vec *args, hook h);
static void ui_updates_win_select     (vec *args, hook h);
static void ui_updates_win_full_redraw(vec *args, hook h);
static void ui_updates_line_change    (vec *args, hook h);
static void ui_updates_line_draw_after(vec *args, hook h);
static void ui_updates_cursor_move    (vec *args, hook h);

int ui_updates_initsys(void)
{
    hook_mount(&win_label_on_caption_set_post, ui_updates_win_label_set, 800);
    hook_mount(&win_label_on_sidebar_set_post, ui_updates_win_label_set, 800);

    hook_mount(&win_on_select, ui_updates_win_select, 800);

    hook_mount(&win_on_offsetx_set,   ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_offsety_set,   ui_updates_win_full_redraw, 800);
    hook_mount(&win_size_on_adj_post, ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_create,        ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_delete_post,   ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_split,         ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_buffer_set,    ui_updates_win_full_redraw, 800);

    hook_mount(&buffer_line_on_change_post, ui_updates_line_change, 800);

    hook_mount(&buffer_line_on_delete_post, ui_updates_line_draw_after, 800);
    hook_mount(&buffer_line_on_insert_post, ui_updates_line_draw_after, 800);
    hook_mount(&buffer_on_batch_region,     ui_updates_line_draw_after, 800);

    hook_mount(&cursor_on_change_pos, ui_updates_cursor_move, 800);

    return 0;
}

int ui_updates_hold(buffer *b)
{
    ui_updates_release();
    ui_updates_holding_buffer = b;

    return 0;
}

int ui_updates_release(void)
{
    win *first, *iter;

    if (ui_updates_holding_buffer == NULL)
        return 0;

    first = win_iter_first(win_root);
    iter = first;
    do {
        if (ui_updates_holding_buffer == win_get_buffer(iter))
            ui_win_draw_subs(iter);

        iter = win_iter_next(iter);
    } while (iter != first);

    ui_updates_holding_buffer = NULL;

    refresh();

    return 0;
}

static void ui_updates_win_label_set(vec *args, hook h)
{
    win *w;

    unpack_arg(0, win, w);
    ui_win_frame_draw_subs(w);

    refresh();
}

static void ui_updates_win_select(vec *args, hook h)
{
    win *w;

    unpack_arg(0, win, w);
    ui_win_frame_draw_subs(w);

    unpack_arg(1, win, w);
    ui_win_frame_draw_subs(w);

    refresh();
}

static void ui_updates_win_full_redraw(vec *args, hook h)
{
    win *w;
    buffer *b;

    unpack_arg(0, win, w);
    b = win_get_buffer(w);

    if (b && ui_updates_holding_buffer == b)
          return;

    ui_win_draw_subs(w);

    refresh();
}

static void ui_updates_line_change(vec *args, hook h)
{
    buffer *b;
    lineno *ln;
    win    *iter, *first;

    unpack_arg(0, buffer, b);
    unpack_arg(1, lineno, ln);

    if (ui_updates_holding_buffer == b)
        return;

    if (ln == NULL)
        return;

    first = win_iter_first(win_root);
    iter = first;
    do {
        if (b == win_get_buffer(iter))
            ui_win_content_draw_line(iter, *ln);

        iter = win_iter_next(iter);
    } while (iter != first);

    refresh();
}

static void ui_updates_line_draw_after(vec *args, hook h)
{
    buffer *b;
    lineno *ln;
    win    *iter, *first;

    unpack_arg(0, buffer, b);
    unpack_arg(1, lineno, ln);

    if (ui_updates_holding_buffer == b)
        return;

    if (ln == NULL)
        return;

    first = win_iter_first(win_root);
    iter = first;
    do {
        if (b == win_get_buffer(iter))
            ui_win_content_draw_lines_after(iter, *ln);

        iter = win_iter_next(iter);
    } while (iter != first);

    refresh();
}

static void ui_updates_cursor_move(vec *args, hook h)
{
    buffer *b;
    cursor *cur;
    win    *iter, *first;
    lineno *oldln, ln;

    unpack_arg(0, cursor, cur);
    unpack_arg(1, lineno, oldln);

    ln = cursor_get_ln(cur);
    b  = cursor_get_buffer(cur);

    if (ui_updates_holding_buffer == b)
        return;

    first = win_iter_first(win_root);
    iter = first;
    do {
        if (b == win_get_buffer(iter))
        {
            ui_win_content_draw_line(iter, ln);
            if (*oldln != ln)
                ui_win_content_draw_line(iter, *oldln);
        }

        iter = win_iter_next(iter);
    } while (iter != first);

    refresh();
}
