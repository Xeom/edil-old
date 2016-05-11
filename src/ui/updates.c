#include <curses.h>

#include "hook.h"
#include "buffer/buffer.h"

#include "win/win.h"
#include "win/label.h"
#include "win/select.h"
#include "win/size.h"
#include "win/iter.h"

#include "ui/ui.h"
#include "ui/win/win.h"
#include "ui/win/frame.h"
#include "ui/win/content.h"

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

int ui_updates_initsys(void)
{
    hook_mount(&win_label_on_caption_set_post, ui_updates_win_label_set, 800);
    hook_mount(&win_label_on_sidebar_set_post, ui_updates_win_label_set, 800);

    hook_mount(&win_on_select, ui_updates_win_select, 800);

    hook_mount(&win_size_on_adj_post, ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_create,        ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_delete_post,   ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_split,         ui_updates_win_full_redraw, 800);
    hook_mount(&win_on_buffer_set,    ui_updates_win_full_redraw, 800);

    hook_mount(&buffer_line_on_change_post, ui_updates_line_change, 800);

    hook_mount(&buffer_line_on_delete_post, ui_updates_line_draw_after, 800);
    hook_mount(&buffer_line_on_insert_post, ui_updates_line_draw_after, 800);
    hook_mount(&buffer_on_batch_region,     ui_updates_line_draw_after, 800);

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

    if (ui_updates_holding_buffer == b)
        return;

    ui_win_draw_subs(w);

    refresh();
}

static void ui_updates_line_change(vec *args, hook h)
{
    buffer *b;
    lineno *ln;
    win    *iter, *last;

    unpack_arg(0, buffer, b);
    unpack_arg(1, lineno, ln);

    if (ui_updates_holding_buffer == b)
        return;

    if (ln == NULL)
        return;

    last = win_iter_last(win_root);
    for (iter = win_iter_first(win_root);
         iter != last;
         iter = win_iter_next(iter))
    {
        if (b == win_get_buffer(iter))
            ui_win_content_draw_line(iter, *ln);
    }

    refresh();
}

static void ui_updates_line_draw_after(vec *args, hook h)
{
        buffer *b;
    lineno *ln;
    win    *iter, *last;

    unpack_arg(0, buffer, b);
    unpack_arg(1, lineno, ln);

    if (ui_updates_holding_buffer == b)
        return;

    if (ln == NULL)
        return;

    last = win_iter_last(win_root);
    for (iter = win_iter_first(win_root);
         iter != last;
         iter = win_iter_next(iter))
    {
        if (b == win_get_buffer(iter))
            ui_win_content_draw_lines_after(iter, *ln);
    }

    refresh();
}
