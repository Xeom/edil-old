#include "buffer/buffer.h"
#include "buffer/deferline.h"
#include "container/table.h"
#include "win/select.h"
#include "ui/face.h"
#include "err.h"

#include "cursor/cursor.h"

struct cursor_s
{
    void        *ptr;
    cursor_type *type;
};

typedef struct buffercont_s buffercont;

struct buffercont_s
{
    vec     cursors;
    size_t  selectind;
};

/* Called when we make a new cursor. *
 * (cursor *cur)                     */
hook_add(cursor_on_spawn, 1);

/* Called before setting the ln of a cursor. The cursor is not affected       *
 * before calling this. Changing *new will change the value the cursor's ln   *
 * is set to. Not called if the relevant cursor_type has no set_ln function.  *
 * (cursor *cur, lineno *new)                                                 */
hook_add(cursor_on_set_ln_pre, 2);
/* Called after setting the ln of a cursor. Not called if there was an error  *
 * while setting the ln, even if cursor_on_set_ln_pre is called. Called even  *
 * if ln was not actually changed.                                            *
 * (cursor *cur, lineno *old)                                                 */
hook_add(cursor_on_set_ln_post, 2);

/* Called before setting the cn of a cursor. The cursor is not affected       *
 * before calling this. Changing *new will change the value the cursor's cn   *
 * is set to. Not called if the relevant cursor_type has no set_cn function.  *
 * (cursor *cur, colno *new)                                                  */
hook_add(cursor_on_set_cn_pre, 2);
/* Called after setting the cn of a cursor with cursor_set_cn.  Not called if *
 * there was an error while setting the cn, even if cursor_on_set_cn_pre is   *
 * called. Called even if ln was not actually changed.                        *
 * (cursor *cur, colno *old)                                                  */
hook_add(cursor_on_set_cn_post, 2);

/* Called before */
hook_add(cursor_on_move_lines_pre, 2);
hook_add(cursor_on_move_lines_post, 3);
hook_add(cursor_on_move_cols_pre, 2);
hook_add(cursor_on_move_cols_post, 3);

hook_add(cursor_on_insert_pre, 2);
hook_add(cursor_on_insert_post, 2);

hook_add(cursor_on_delete_pre, 2);
hook_add(cursor_on_delete_post, 2);

hook_add(cursor_on_enter_pre, 1);
hook_add(cursor_on_enter_post, 1);

hook_add(cursor_on_change_pos, 2);

table cursors_by_buffer;

void cursor_handle_deferline_draw(vec *args, hook h);

int cursor_initsys(void)
{
    table_create(&cursors_by_buffer, sizeof(buffercont), sizeof(buffer *),
                 NULL, NULL, NULL);

    hook_mount(&buffer_deferline_on_draw, &cursor_handle_deferline_draw, 500);

    return 0;
}

static buffercont *cursor_buffercont_init(buffer *b)
{
    buffercont *rtn;

    table_set(&cursors_by_buffer, &b, NULL);
    rtn = table_get(&cursors_by_buffer, &b);

    vec_create(&(rtn->cursors), sizeof(cursor));
    rtn->selectind = 0;

    return rtn;
}

cursor *cursor_spawn(buffer *b, cursor_type *type)
{
    buffercont *cont;
    cursor     *new;

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        cont = cursor_buffercont_init(b);

    vec_insert_end(&(cont->cursors), 1, NULL);
    new = vec_item(&(cont->cursors), vec_len(&(cont->cursors)) - 1);

    new->type = type;

    fprintf(stderr, "NEW %p TYPE %p\n", new, type);

    ASSERT_PTR(type->init, critical, return NULL);
    new->ptr = type->init(b);

    hook_call(cursor_on_spawn, new);

    return new;
}

lineno cursor_get_ln(cursor *cur)
{
    if (!(cur->type->get_ln))
        return 0;

    else
        return cur->type->get_ln(cur->ptr);
}

colno cursor_get_cn(cursor *cur)
{
    if (!(cur->type->get_cn))
        return 0;

    else
        return cur->type->get_cn(cur->ptr);
}

buffer *cursor_get_buffer(cursor *cur)
{
    ASSERT_PTR(cur->type->get_buffer,
               critical, return NULL);

    return cur->type->get_buffer(cur->ptr);
}

int cursor_set_ln(cursor *cur, lineno ln)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->set_ln))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_set_ln_pre, cur, &ln);

    ASSERT_INT(cur->type->set_ln(cur->ptr, ln),
               high, return -1);

    hook_call(cursor_on_set_ln_post, cur, &oldln);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}

int cursor_set_cn(cursor *cur, colno cn)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->set_cn))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_set_cn_pre, cur, &cn);

    ASSERT_INT(cur->type->set_cn(cur->ptr, cn),
               high, return -1);

    hook_call(cursor_on_set_cn_post, cur, &oldcn);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}

int cursor_move_cols(cursor *cur, int n)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->move_cols))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_move_cols_pre, cur, &n);

    ASSERT_INT(cur->type->move_cols(cur->ptr, n),
               high, return -1);

    hook_call(cursor_on_move_cols_post, cur, &oldln, &oldcn);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}

int cursor_move_lines(cursor *cur, int n)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->move_lines))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_move_lines_pre, cur, &n);

    ASSERT_INT(cur->type->move_lines(cur->ptr, n),
               high, return -1);

    hook_call(cursor_on_move_lines_post, cur, &oldln, &oldcn);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}
#include <stdio.h>
int cursor_insert(cursor *cur, const char *str)
{
    lineno oldln;
    lineno oldcn;
    fprintf(stderr, "CUR %p\n", cur);
//    if (!(cur->type))
    //      abort();
    if (!(cur->type->insert))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_insert_pre, cur, str);

    ASSERT_INT(cur->type->insert(cur->ptr, str),
               high, return -1);

    hook_call(cursor_on_insert_post, cur, str);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}

int cursor_delete(cursor *cur, uint n)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->delete))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_delete_pre, cur, &n);

    ASSERT_INT(cur->type->delete(cur->ptr, n),
               high, return -1);

    hook_call(cursor_on_delete_post, cur, &n);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}

int cursor_enter(cursor *cur)
{
    lineno oldln;
    colno  oldcn;

    if (!(cur->type->enter))
        return 0;

    oldln = cursor_get_ln(cur);
    oldcn = cursor_get_cn(cur);

    hook_call(cursor_on_enter_pre, cur);

    ASSERT_INT(cur->type->enter(cur->ptr),
               high, return -1);

    hook_call(cursor_on_enter_post, cur);

    if (oldln != cursor_get_ln(cur) ||
        oldcn != cursor_get_cn(cur))
        hook_call(cursor_on_change_pos, cur, &oldln, &oldcn);

    return 0;
}


cursor *cursor_buffer_selected(buffer *b)
{
    buffercont *cont;

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        return NULL;

    return vec_item(&(cont->cursors), cont->selectind);
}

vec *cursor_buffer_all(buffer *b)
{
    buffercont *cont;

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        return NULL;

    return &(cont->cursors);
}

cursor *cursor_selected(void)
{
    buffer *b;
    win    *w;

    w = win_select_get();
    b = win_get_buffer(w);

    return cursor_buffer_selected(b);
}

#define unpack_arg(num, type, name)             \
    {                                           \
        if (vec_item(args, num) == NULL)        \
            return;                             \
        name = *(type **)vec_item(args, num);   \
    }

void cursor_handle_deferline_draw(vec *args, hook h)
{
    win       *w;
    lineno    *ln;
    deferline *dl;
    cursor    *selected;

    unpack_arg(0, win,       w);
    unpack_arg(2, lineno,    ln);
    unpack_arg(3, deferline, dl);

    if (w != win_select_get())
        return;

    selected = cursor_selected();

    if (selected == NULL)
        return;

    if (*ln != cursor_get_ln(selected))
        return;


    buffer_deferline_insert(dl, cursor_get_cn(selected), face_cursor);
}
