#include <stdlib.h>

#include "cursor/point.h"
#include "ui/face.h"
#include "buffer/deferline.h"
#include "win/win.h"
#include "hook.h"

#include "cursor/region.h"

struct region_s
{
    int    active;
    point *runner;
    point *anchor;
};

face cursor_region_anchor_face = {yellow, black, 0, 0};

static void *cursor_region_init_t(buffer *b)
{   return cursor_region_init(b); }

static int cursor_region_free_t(void *ptr)
{   return cursor_region_free(ptr); }

static lineno cursor_region_get_ln_t(void *ptr)
{   return cursor_region_get_ln(ptr); }

static colno cursor_region_get_cn_t(void *ptr)
{   return cursor_region_get_cn(ptr); }

static int cursor_region_set_ln_t(void *ptr, lineno ln)
{   return cursor_region_set_ln(ptr, ln); }

static int cursor_region_set_cn_t(void *ptr, colno cn)
{   return cursor_region_set_cn(ptr, cn); }

static int cursor_region_move_cols_t(void *ptr, int n)
{   return cursor_region_move_cols(ptr, n); }

static int cursor_region_move_lines_t(void *ptr, int n)
{   return cursor_region_move_lines(ptr, n); }

static int cursor_region_insert_t(void *ptr, const char *str)
{   return cursor_region_insert(ptr, str); }

static int cursor_region_delete_t(void *ptr, uint n)
{   return cursor_region_delete(ptr, n); }

static int cursor_region_enter_t(void *ptr)
{   return cursor_region_enter(ptr); }

static int cursor_region_activate_t(void *ptr)
{   return cursor_region_activate(ptr); }

static int cursor_region_deactivate_t(void *ptr)
{   return cursor_region_deactivate(ptr); }

/* A cursor_type representing a region */
cursor_type cursor_region_type =
{
    &cursor_region_init_t,
    &cursor_region_free_t,
    &cursor_region_get_ln_t,
    &cursor_region_get_cn_t,
    &cursor_region_set_ln_t,
    &cursor_region_set_cn_t,
    &cursor_region_move_lines_t,
    &cursor_region_move_cols_t,
    &cursor_region_insert_t,
    &cursor_region_delete_t,
    &cursor_region_enter_t,
    &cursor_region_activate_t,
    &cursor_region_deactivate_t
};

void cursor_region_handle_deferline_draw(vec *args, hook h);

int cursor_region_initsys(void)
{
    hook_mount(&buffer_deferline_on_draw,
               &cursor_region_handle_deferline_draw, 500);

    return 0;
}

region *cursor_region_init(buffer *b)
{
    region *rtn;

    rtn = malloc(sizeof(region));

    rtn->active = 0;
    rtn->runner = cursor_point_init(b);
    rtn->anchor = cursor_point_init(b);

    return rtn;
}

int cursor_region_free(region *r)
{
    cursor_point_free(r->runner);
    cursor_point_free(r->anchor);

    free(r);

    return 0;
}

static int cursor_region_delete_contents(region *r)
{
    long diff;
    point *start, *end;

    if (!(r->active))
        return 0;

    if (cursor_point_cmp(r->runner, r->anchor) == 1)
    {
        end   = r->runner;
        start = r->anchor;
    }
    else
    {
        start = r->runner;
        end   = r->anchor;
    }

    diff = cursor_point_sub(start, end);

    while (diff > 0xffff)
    {
        diff -= 0xffff;
        cursor_point_delete(end, 0xffff);
    }

    r->active = 0;
    cursor_point_delete(end, (uint)diff);

    return 0;
}

lineno cursor_region_get_ln(region *r)
{
    return cursor_point_get_ln(r->runner);
}

colno cursor_region_get_cn(region *r)
{
    return cursor_point_get_cn(r->runner);
}

int cursor_region_set_ln(region *r, lineno ln)
{
    return cursor_point_set_ln(r->runner, ln);
}

int cursor_region_set_cn(region *r, colno cn)
{
    return cursor_point_set_cn(r->runner, cn);
}

int cursor_region_move_cols(region *r, int n)
{
    return cursor_point_move_cols(r->runner, n);
}

int cursor_region_move_lines(region *r, int n)
{
    return cursor_point_move_lines(r->runner, n);
}

int cursor_region_insert(region *r, const char *str)
{
    cursor_region_delete_contents(r);
    cursor_point_insert(r->runner, str);

    return 0;
}

int cursor_region_delete(region *r, uint n)
{
    if (r->active)
    {
        n--;
        cursor_region_delete_contents(r);
    }

    if (n)
        cursor_point_delete(r->runner, n);

    return 0;
}

int cursor_region_enter(region *r)
{
    cursor_region_delete_contents(r);
    cursor_point_enter(r->runner);

    return 0;
}

int cursor_region_activate(region *r)
{
    cursor_point_set_ln(r->anchor, cursor_point_get_ln(r->runner));
    cursor_point_set_cn(r->anchor, cursor_point_get_cn(r->runner));

    r->active = 1;

    return 0;
}

int cursor_region_deactivate(region *r)
{
    r->active = 0;

    return 0;
}

#define unpack_arg(num, type, name)             \
    {                                           \
        if (vec_item(args, num) == NULL)        \
            return;                             \
        name = *(type **)vec_item(args, num);   \
    }

void cursor_region_handle_deferline_draw(vec *args, hook h)
{
    win       *w;
    lineno    *ln;
    deferline *dl;
    cursor    *selected;
    region    *r;
    char      *serial;

    unpack_arg(0, win,       w);
    unpack_arg(2, lineno,    ln);
    unpack_arg(3, deferline, dl);

    if (w != win_select_get())
        return;

    selected = cursor_selected();

    if (selected == NULL)
        return;

    if (cursor_get_type(selected) != &cursor_region_type)
        return;

    r = cursor_get_ptr(selected);

    if (!(r->active))
        return;

    if (*ln != cursor_point_get_ln(r->anchor))
        return;

    serial = ui_face_serialize(cursor_region_anchor_face, 1);
    buffer_deferline_insert(dl, cursor_point_get_cn(r->anchor), serial);
    free(serial);
}
