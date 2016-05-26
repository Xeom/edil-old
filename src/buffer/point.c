#include <stdlib.h>
#include <string.h>

#include "container/vec.h"
#include "buffer/buffer.h"
#include "hook.h"
#include "err.h"

#include "buffer/point.h"

struct point_s
{
    lineno ln;
    colno  cn;
    buffer *b;
};

static table buffer_point_all;

static int buffer_point_move_ln(point *p, lineno ln);
static int buffer_point_fix_cn(point *p);
static int buffer_point_fix_ln(point *p);

static int buffer_point_move_cols_backward(point *p, uint n);
static int buffer_point_move_cols_forward(point *p, uint n);

static int buffer_point_move_lines_backward(point *p, uint n);
static int buffer_point_move_lines_forward(point *p, uint n);

static long buffer_point_sub_abs(point *larger, point *smaller);

static void buffer_point_handle_line_delete(vec *args, hook h);
static void buffer_point_handle_line_insert(vec *args, hook h);
static void buffer_point_handle_line_change(vec *args, hook h);

hook_add(buffer_point_on_move_pre,  1);
hook_add(buffer_point_on_move_post, 3);

int buffer_point_initsys(void)
{
    TRACE_PTR(table_create(&buffer_point_all,
                            sizeof(table), sizeof(buffer *),
                            NULL, NULL, NULL),
              return -1);

    TRACE_INT(hook_mount(&buffer_line_on_delete_post,
                          buffer_point_handle_line_delete, 600),
              return -1);

    TRACE_INT(hook_mount(&buffer_line_on_insert_post,
                          buffer_point_handle_line_insert, 600),
              return -1);

    TRACE_INT(hook_mount(&buffer_line_on_change_post,
                          buffer_point_handle_line_change, 600),
              return -1);

    return 0;
}

static size_t point_invalid_index = INVALID_INDEX;

point *buffer_point_init(buffer *b, lineno ln, colno cn)
{
    table *subtable;
    vec   *subvec;
    point *rtn;

    ASSERT_PTR(b, high, return NULL);
    ASSERT_PTR(rtn = malloc(sizeof(point)), terminal, return NULL);

    rtn->ln = ln;
    rtn->cn = cn;
    rtn->b  = b;

    buffer_point_fix_ln(rtn);
    buffer_point_fix_cn(rtn);

    subtable = table_get(&buffer_point_all, &b);

    if (subtable == NULL)
    {
        TRACE_INT(table_set(&buffer_point_all, &b, NULL),
                  free(rtn);
                  return NULL);
        TRACE_PTR(subtable = table_get(&buffer_point_all, &b),
                  free(rtn);
                  return NULL);
        TRACE_PTR(table_create(subtable, sizeof(vec), sizeof(lineno),
                               NULL, NULL, (void *)&point_invalid_index),
                  free(rtn);
                  return NULL);
    }

    subvec = table_get(subtable, &(rtn->ln));

    if (subvec == NULL)
    {
        TRACE_INT(table_set(subtable, &(rtn->ln), NULL),
                  free(rtn);
                  return NULL);
        TRACE_PTR(subvec = table_get(subtable, &(rtn->ln)),
                  free(rtn);
                  return NULL);
        TRACE_PTR(vec_create(subvec, sizeof(point *)),
                  free(rtn);
                  return NULL);
    }

    TRACE_INT(vec_insert_end(subvec, 1, &rtn),
              free(rtn);
              return NULL);

    return rtn;
}

static int buffer_point_move_ln(point *p, lineno ln)
{
    table *subtable;
    vec   *subvec;

    if (ln == p->ln)
        return 0;

    ASSERT_PTR(subtable = table_get(&buffer_point_all, &(p->b)),
               critical, return -1);
    ASSERT_PTR(subvec   = table_get(subtable, &(p->ln)),
               critical, return -1);

    ASSERT_INT(vec_delete(subvec, vec_find(subvec, &p), 1),
               critical, return -1);

    if (vec_len(subvec) == 0)
    {
        vec_kill(subvec);
        ASSERT_INT(table_delete(subtable, &(p->ln)),
                   critical, return -1);
    }

    subvec = table_get(subtable, &ln);

    if (subvec == NULL)
    {
        ASSERT_INT(table_set(subtable, &ln, NULL),
                   critical, return -1);
        ASSERT_PTR(subvec = table_get(subtable, &ln),
                   critical, return -1);
        ASSERT_PTR(vec_create(subvec, sizeof(point *)),
                   critical, return -1);
    }

    ASSERT_INT(vec_insert_end(subvec, 1, &p),
               critical, return -1);

    p->ln = ln;

    TRACE_INT(buffer_point_fix_cn(p),
              return -1);

    return 0;
}

static int buffer_point_fix_cn(point *p)
{
    size_t linelen;

    linelen = buffer_len_line(p->b, p->ln);

    if (p->cn > linelen)
        p->cn = linelen;

    return 0;
}

static int buffer_point_fix_ln(point *p)
{
    size_t buflen;

    buflen = buffer_len(p->b);

    if (buflen == 0)
        TRACE_INT(buffer_point_move_ln(p, 0),
                  return -1);
    else if (p->ln >= buflen)
        TRACE_INT(buffer_point_move_ln(p, buflen - 1),
                  return -1);

    return 0;
}

static int buffer_point_move_cols_backward(point *p, uint n)
{
    lineno ln;

    ln = p->ln;

    while (n > p->cn && p->ln)
    {
        --ln;
        n -= (uint)p->cn + 1;
        p->cn = buffer_len_line(p->b, ln);
    }

    if (n > p->cn)
        p->cn = 0;
    else
        p->cn -= n;

    TRACE_INT(buffer_point_move_ln(p, ln),
              return -1);

    return 0;
}

static int buffer_point_move_cols_forward(point *p, uint n)
{
    size_t buflen;
    size_t linelen;
    lineno ln;

    buflen = buffer_len(p->b);
    ln     = p->ln;

    while (p->ln < buflen - 1)
    {
        linelen = buffer_len_line(p->b, ln);

        if (n <= linelen - p->cn)
            break;

        n -= (uint)linelen - (uint)p->cn + 1;
        ++ln;
        p->cn  = 0;
    }

    linelen = buffer_len_line(p->b, ln);
    p->cn += n;

    if (p->cn > linelen)
        p->cn = linelen;

    TRACE_INT(buffer_point_move_ln(p, ln),
              return -1);

    return 0;
}

int buffer_point_move_cols(point *p, int n)
{
    int    rtn;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(buffer_point_on_move_pre, p);

    if (n < 0)
        rtn = buffer_point_move_cols_backward(p, (uint)-n);

    else if (n > 0)
        rtn = buffer_point_move_cols_forward(p, (uint)n);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);

    return rtn;
}

static int buffer_point_move_lines_forward(point *p, uint n)
{
    size_t buflen;

    buflen = buffer_len(p->b);

    if (n + p->ln >= buflen)
        TRACE_INT(buffer_point_move_ln(p, buflen - 1),
                  return -1);
    else
        TRACE_INT(buffer_point_move_ln(p, p->ln + n),
                  return -1);

    TRACE_INT(buffer_point_fix_cn(p),
              return -1);

    return 0;
}

static int buffer_point_move_lines_backward(point *p, uint n)
{
    if (n >= p->ln)
        TRACE_INT(buffer_point_move_ln(p, 0),
                  return -1);
    else
        TRACE_INT(buffer_point_move_ln(p, p->ln - n),
                  return -1);

    return 0;
}

int buffer_point_move_lines(point *p, int n)
{
    int rtn;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(buffer_point_on_move_pre, p);

    if (n < 0)
        rtn = buffer_point_move_lines_backward(p, (uint)-n);
    else if (n > 0)
        rtn = buffer_point_move_lines_forward(p, (uint)n);

    buffer_point_fix_cn(p);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);

    return rtn;
}

int buffer_point_delete(point *p, uint n)
{
    vec   *origline, *newline;
    colno  origcn;
    lineno origln;

    origline = buffer_get_line(p->b, p->ln);
    origcn   = p->cn;
    origln   = p->ln;

    hook_call(buffer_point_on_move_pre, p);

    if (origline == NULL)
        return -1;

    if (n > 10)
        buffer_batch_start(p->b);

    while (n > p->cn && p->ln)
    {
        buffer_point_move_ln(p, p->ln - 1);
        n -= (uint)p->cn + 1;
        buffer_delete(p->b, p->ln + 1);
        p->cn = buffer_len_line(p->b, p->ln);
    }

    newline = buffer_get_line(p->b, p->ln);
    n = MIN(n, (uint)p->cn);

    if (newline == NULL)
    {
        vec_free(origline);
        buffer_batch_end(p->b);

        return -1;
    }

    vec_delete(newline, p->cn - n, vec_len(newline) - p->cn + n);
    vec_insert_end(newline, vec_len(origline) - origcn,
                   vec_item(origline, origcn));

    p->cn -= n;

    buffer_set_line(p->b, p->ln, newline);

    buffer_batch_end(p->b);
    hook_call(buffer_point_on_move_post, p, &origln, &origcn);

    vec_free(origline);
    vec_free(newline);


    return 0;
}

int buffer_point_insert(point *p, char *str)
{
    size_t inslen;
    vec   *l;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    if (buffer_len(p->b) == 0)
        buffer_insert(p->b, p->ln);

    inslen = strlen(str);
    l      = buffer_get_line(p->b, p->ln);

    hook_call(buffer_point_on_move_pre, p);

    vec_insert(l, p->cn, inslen, str);

    p->cn += inslen;
    buffer_set_line(p->b, p->ln, l);

    vec_free(l);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);

    return 0;
}

int buffer_point_enter(point *p)
{
    lineno newln, origln;
    vec *start, *end;
    colno origcn;

    ASSERT_PTR(p, high, return -1);

    TRACE_PTR(start = buffer_get_line(p->b, p->ln),
              return -1);

    origln = p->ln;
    origcn = p->cn;

    if (start == NULL)
        return -1;

    TRACE_PTR(end = vec_cut(start, p->cn, vec_len(start) - p->cn),
              vec_free(start);
              return -1);
    TRACE_INT(vec_delete(start,    p->cn, vec_len(start) - p->cn),
              vec_free(start);
              vec_free(end);
              return -1);

    hook_call(buffer_point_on_move_pre, p);

    TRACE_INT(buffer_set_line(p->b, p->ln, start),
              vec_free(start);
              vec_free(end);
              return -1);

    newln = p->ln + 1;
    p->cn = 0;

    TRACE_INT(buffer_insert(p->b, newln),
              vec_free(start);
              vec_free(end);
              return -1);
    TRACE_INT(buffer_set_line(p->b, newln, end),
              vec_free(start);
              vec_free(end);
              return -1);

    TRACE_INT(buffer_point_move_ln(p, newln),
              vec_free(start);
              vec_free(end);
              return -1);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);

    vec_free(start);
    vec_free(end);

    return 0;
}

lineno buffer_point_get_ln(point *p)
{
    return p->ln;
}

void buffer_point_set_ln(point *p, lineno ln)
{
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(buffer_point_on_move_pre, p);

    buffer_point_move_ln(p, ln);

    buffer_point_fix_ln(p);
    buffer_point_fix_cn(p);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);
}

colno buffer_point_get_cn(point *p)
{
    return p->cn;
}

void buffer_point_set_cn(point *p, lineno cn)
{
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(buffer_point_on_move_pre, p);

    p->cn = cn;
    buffer_point_fix_cn(p);

    hook_call(buffer_point_on_move_post, p, &origln, &origcn);
}

buffer *buffer_point_get_buffer(point *p)
{
    return p->b;
}

int buffer_point_cmp(point *a, point *b)
{
    if (a->ln == b->ln)
    {
        if (a->cn == b->cn)
            return 0;

        return (a->cn > b->cn) ? 1 : -1;
    }

    return (a->ln > b->ln) ? 1 : -1;
}

static long buffer_point_sub_abs(point *larger, point *smaller)
{
    lineno  ln;
    long    sum;
    buffer *b;

    b = larger->b;
    sum = 0;

    for (ln = smaller->ln; ln < larger->ln; ln++)
        sum += (long)buffer_len_line(b, ln) + 1;

    sum -= (long)smaller->cn;
    sum += (long)larger->cn;

    return sum;
}

long buffer_point_sub(point *a, point *b)
{
    int cmp;

    if (a->b != b->b)
        return 0;

    cmp = buffer_point_cmp(a, b);

    if (cmp == 1)
        return buffer_point_sub_abs(a, b);
    else if (cmp == -1)
        return -buffer_point_sub_abs(b, a);
    else
        return 0;
}

static void buffer_point_handle_line_delete(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table  *subtable;
    vec    *tomove;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&buffer_point_all, &b);

    if (subtable == NULL)
        return;

    tomove = vec_init(sizeof(point *));

    table_foreach_ptr(subtable, vec *, v,
                      vec_foreach(v, point *, p,
                                  if (p->ln < ln || p->ln == 0)
                                      break;

                                  vec_insert_end(tomove, 1, &p);
                          );
        );

    vec_foreach(tomove, point *, p,
                lineno origln;
                colno  origcn;

                origln = p->ln;
                origcn = p->cn;

                hook_call(buffer_point_on_move_pre, p);

                buffer_point_move_ln(p, p->ln - 1);
                buffer_point_fix_ln(p);

                hook_call(buffer_point_on_move_post, p, &origln, &origcn);
        );

    vec_free(tomove);
}

static void buffer_point_handle_line_change(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table *subtable;
    vec   *subvec;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&buffer_point_all, &b);

    if (subtable == NULL)
        return;

    subvec = table_get(subtable, &ln);

    if (subvec == NULL)
        return;

    vec_foreach(subvec, point *, p,
                lineno origln;
                colno  origcn;

                origln = p->ln;
                origcn = p->cn;

                hook_call(buffer_point_on_move_pre, p);

                buffer_point_fix_cn(p);

                hook_call(buffer_point_on_move_post, p, &origln, &origcn);

        );
}

static void buffer_point_handle_line_insert(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table  *subtable;
    vec    *tomove;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&buffer_point_all, &b);

    if (subtable == NULL)
        return;

    tomove = vec_init(sizeof(point *));

    table_foreach_ptr(subtable, vec *, v,
                      fprintf(stderr, "ln %lu\n", vec_len(v));
                      vec_foreach(v, point *, p,
                                  if (p->ln < ln)
                                      continue;
                                  vec_insert_end(tomove, 1, &p);
                          );
        );

    vec_foreach(tomove, point *, p,
                lineno origln;
                colno  origcn;

                origln = p->ln;
                origcn = p->cn;

                hook_call(buffer_point_on_move_pre, p);

                if (p->ln + 1 < buffer_len(p->b))
                    buffer_point_move_ln(p, p->ln + 1);

                hook_call(buffer_point_on_move_post, p, &origln, &origcn);
        );

    vec_free(tomove);
}
