#include "buffer/buffer.h"
#include "container/table.h"
#include "win/select.h"
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

table cursors_by_buffer;

int cursor_initsys(void)
{
    table_create(&cursors_by_buffer, sizeof(buffercont), sizeof(buffer *),
                 NULL, NULL, NULL);

    return 0;
}

static buffercont *cursor_buffercont_init(buffer *b)
{
    buffercont *rtn;

    table_set(&cursors_by_buffer, &b, NULL);
    rtn = table_get(&cursors_by_buffer, &b);

    vec_create(&(rtn->cursors), sizeof(cursor));
    rtn->selectind = INVALID_INDEX;

    return rtn;
}

cursor *cursor_spawn(buffer *b, cursor_type *type)
{
    buffercont *cont;
    cursor     *new;

    cont = table_get(&cursors_by_buffer, &b);

    if (!cont)
        cont = cursor_buffercont_init(b);

    vec_insert_end(&(cont->cursors), 0, NULL);
    new = vec_item(&(cont->cursors), vec_len(&(cont->cursors)));

    new->type = type;

    ASSERT_PTR(type->init, critical, return NULL);
    new->ptr = type->init(b);

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
    if (!(cur->type->set_ln))
        return 0;

    return cur->type->set_ln(cur->ptr, ln);
}

int cursor_set_cn(cursor *cur, colno cn)
{
    if (!(cur->type->set_cn))
        return 0;

    return cur->type->set_cn(cur->ptr, cn);
}

int cursor_move_cols(cursor *cur, int n)
{
    if (!(cur->type->move_cols))
        return 0;

    return cur->type->move_cols(cur->ptr, n);
}

int cursor_move_lines(cursor *cur, int n)
{
    if (!(cur->type->move_lines))
        return 0;

    return cur->type->move_lines(cur->ptr, n);
}

int cursor_insert(cursor *cur, char *str)
{
    if (!(cur->type->insert))
        return 0;

    return cur->type->insert(cur->ptr, str);
}

int cursor_delete(cursor *cur, uint n)
{
    if (!(cur->type->delete))
        return 0;

    return cur->type->delete(cur->ptr, n);
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

cursor *cursor_current(void)
{
    buffer *b;
    win    *w;

    w = win_select_get();
    b = win_get_buffer(w);

    return cursor_buffer_selected(b);
}


