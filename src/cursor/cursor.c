#include "buffer/buffer.h"
#include "buffer/deferline.h"

#include "container/table.h"
#include "win/select.h"
#include "ui/face.h"
#include "err.h"

#include "cursor/snap.h"

#include "cursor/cursor.h"

struct cursor_s
{
    buffer      *b;
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

hook_add(cursor_on_free, 1);

hook_add(cursor_on_select, 1);

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

hook_add(cursor_on_activate_pre, 1);
hook_add(cursor_on_activate_post, 1);

hook_add(cursor_on_deactivate_pre, 1);
hook_add(cursor_on_deactivate_post, 1);

hook_add(cursor_on_change_pos, 2);

table cursors_by_buffer;

void cursor_handle_deferline_draw(vec *args, hook h);

int cursor_initsys(void)
{
    TRACE_PTR(table_create(
                  &cursors_by_buffer, sizeof(buffercont), sizeof(buffer *),
                  NULL, NULL, NULL),
              return -1);

    TRACE_INT(hook_mount(&buffer_deferline_on_draw,
                          &cursor_handle_deferline_draw, 500),
               return -1);

    TRACE_INT(cursor_snap_initsys(), return -1);

    return 0;
}

static buffercont *cursor_buffercont_init(buffer *b)
{
    buffercont *rtn;

    TRACE_INT(table_set(&cursors_by_buffer, &b, NULL),
              return NULL);

    TRACE_PTR(rtn = table_get(&cursors_by_buffer, &b),
              table_delete(&cursors_by_buffer, &b);
              return NULL);

    TRACE_PTR(vec_create(&(rtn->cursors), sizeof(cursor)),
              table_delete(&cursors_by_buffer, &b);
              return NULL);

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

    TRACE_INT(vec_insert_end(&(cont->cursors), 1, NULL),
              return NULL);
    TRACE_PTR(new = vec_item(&(cont->cursors), vec_len(&(cont->cursors)) - 1),
              return NULL);

    new->type = type;

    ASSERT_PTR(type->init, critical, return NULL);
    new->ptr = type->init(b);
    new->b   = b;

    hook_call(cursor_on_spawn, new);

    return new;
}

int cursor_free(cursor *cur)
{
    buffercont *cont;
    buffer *b;
    size_t  ind;

    ASSERT_PTR(cur, high, return -1);

    b = cur->b;

    TRACE_PTR(cont = table_get(&cursors_by_buffer, &b), return -1);

    if (cur->type && cur->type->free)
        cur->type->free(cur->ptr);

    hook_call(cursor_on_free, cur);

    ind = (size_t)((char *)cur - (char *)vec_item(&(cont->cursors), 0))
        / sizeof(cursor);

    if (cont->selectind && cont->selectind >= ind)
        cont->selectind--;

    TRACE_INT(vec_delete(&(cont->cursors), ind, 1),
              return -1);

    return 0;
}

buffer *cursor_get_buffer(cursor *cur)
{
    ASSERT_PTR(cur, high, return NULL);

    return cur->b;
}

void *cursor_get_ptr(cursor *cur)
{
    ASSERT_PTR(cur, high, return NULL);

    return cur->ptr;
}

cursor_type *cursor_get_type(cursor *cur)
{
    ASSERT_PTR(cur, high, return NULL);

    return cur->type;
}

lineno cursor_get_ln(cursor *cur)
{
    ASSERT_PTR(cur,       high,     return INVALID_INDEX);
    ASSERT_PTR(cur->type, critical, return INVALID_INDEX);

    if (!(cur->type->get_ln))
        return INVALID_INDEX;

    else
        return cur->type->get_ln(cur->ptr);
}

colno cursor_get_cn(cursor *cur)
{
    ASSERT_PTR(cur,       high,     return INVALID_INDEX);
    ASSERT_PTR(cur->type, critical, return INVALID_INDEX);

    if (!(cur->type->get_cn))
        return 0;

    else
        return cur->type->get_cn(cur->ptr);
}

int cursor_set_ln(cursor *cur, lineno ln)
{
    lineno oldln;
    colno  oldcn;

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

int cursor_insert(cursor *cur, const char *str)
{
    lineno oldln;
    lineno oldcn;

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

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

int cursor_activate(cursor *cur)
{
    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

    if (!(cur->type->activate))
        return 0;

    hook_call(cursor_on_activate_pre, cur);

    ASSERT_INT(cur->type->activate(cur->ptr),
               high, return -1);

    hook_call(cursor_on_activate_post, cur);

    return 0;
}

int cursor_deactivate(cursor *cur)
{
    ASSERT_PTR(cur,       high,     return -1);
    ASSERT_PTR(cur->type, critical, return -1);

    if (!(cur->type->deactivate))
        return 0;

    hook_call(cursor_on_deactivate_pre, cur);

    ASSERT_INT(cur->type->deactivate(cur->ptr),
               high, return -1);

    hook_call(cursor_on_deactivate_post, cur);

    return 0;
}

cursor *cursor_buffer_selected(buffer *b)
{
    buffercont *cont;

    ASSERT_PTR(b, high, return NULL);

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        return NULL;

    return vec_item(&(cont->cursors), cont->selectind);
}

vec *cursor_buffer_all(buffer *b)
{
    buffercont *cont;

    ASSERT_PTR(b, high, return NULL);

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        return NULL;

    return &(cont->cursors);
}

cursor *cursor_selected(void)
{
    buffer *b;
    win    *w;

    TRACE_PTR(w = win_select_get(),  return NULL);
    TRACE_PTR(b = win_get_buffer(w), return NULL);

    return cursor_buffer_selected(b);
}

int cursor_select_next(buffer *b)
{
    cursor     *oldselected;
    buffercont *cont;

    oldselected = cursor_buffer_selected(b);

    TRACE_PTR(cont = table_get(&cursors_by_buffer, &b), return -1);

    cont->selectind += 1;
    cont->selectind %= vec_len(&(cont->cursors));

    hook_call(cursor_on_select, b, oldselected);

    return 0;
}

int cursor_select_last(buffer *b)
{
    cursor     *oldselected;
    buffercont *cont;

    oldselected = cursor_buffer_selected(b);

    TRACE_PTR(cont = table_get(&cursors_by_buffer, &b), return -1);

    cont->selectind = vec_len(&(cont->cursors)) - 1;

    hook_call(cursor_on_select, b, oldselected);

    return 0;
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
