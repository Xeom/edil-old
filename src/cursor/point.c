#include <stdlib.h>
#include <string.h>

#include "container/vec.h"
#include "buffer/buffer.h"
#include "cursor/cursor.h"
#include "hook.h"
#include "err.h"

#include "cursor/point.h"

struct point_s
{
    lineno ln;
    colno  cn;
    buffer *b;
};

/* A series of methods that simply alias cursor_point functions to functions  *
 * appropriate types for use in a cursor_type. This method was used over      *
 * simply casting the function pointers due to its far higher resistance to   *
 * errors. Also casting function pointers to different types is less well     *
 * defined behaviour.                                                         */
static inline void *cursor_point_init_t(buffer *b)
{   return cursor_point_init(b); }

static inline int cursor_point_free_t(void *ptr)
{   return cursor_point_free(ptr); }

static inline lineno cursor_point_get_ln_t(void *ptr)
{   return cursor_point_get_ln(ptr); }

static inline colno cursor_point_get_cn_t(void *ptr)
{   return cursor_point_get_cn(ptr); }

static inline int cursor_point_set_ln_t(void *ptr, lineno ln)
{   return cursor_point_set_ln(ptr, ln); }

static inline int cursor_point_set_cn_t(void *ptr, colno cn)
{   return cursor_point_set_cn(ptr, cn); }

static inline int cursor_point_move_cols_t(void *ptr, int n)
{   return cursor_point_move_cols(ptr, n); }

static inline int cursor_point_move_lines_t(void *ptr, int n)
{   return cursor_point_move_lines(ptr, n); }

static inline int cursor_point_insert_t(void *ptr, const char *str)
{   return cursor_point_insert(ptr, str); }

static inline int cursor_point_delete_t(void *ptr, uint n)
{   return cursor_point_delete(ptr, n); }

static inline int cursor_point_enter_t(void *ptr)
{   return cursor_point_enter(ptr); }

/* A cursor_type representing a point */
cursor_type cursor_point_type =
{
    &cursor_point_init_t,
    &cursor_point_free_t,
    &cursor_point_get_ln_t,
    &cursor_point_get_cn_t,
    &cursor_point_set_ln_t,
    &cursor_point_set_cn_t,
    &cursor_point_move_lines_t,
    &cursor_point_move_cols_t,
    &cursor_point_insert_t,
    &cursor_point_delete_t,
    &cursor_point_enter_t,
    /* Points do not activate, so these methods are left null */
    NULL,
    NULL
};

/*
 * A table to keep track of the locations of all points currently in existance.
 *
 * table{ buffer -> table{ lineno -> vec { point * } } }
 *
 */
static table cursor_point_all;

/*
 * Updates things that map lineno->point to tell them that a point has moved to
 * a new linenumber. Should be used to change struct point_s -> ln always.
 *
 * @param p  The point to move.
 * @param ln To linenumber to move the point to.
 *
 * @return   0 on success, -1 on error.
 *
 */
static int cursor_point_move_ln(point *p, lineno ln);

/*
 * Fix either the line or column number of a point. These should be used when it
 * is uncertain whether a point is in a valid position to move it into a valid
 * position. When there is no valid position, they set either cn or ln to 0.
 * Note that fixing ln may move a point to a new line, and so fix_cn should be
 * called after calls to fix_ln in case this new line is too short for the
 * point.
 *
 * @param p The point to fix the position of.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int cursor_point_fix_cn(point *p);
static int cursor_point_fix_ln(point *p);

/*
 * Move a point a number of characters either forward or backward.
 * cursor_point_move_cols wraps these, calling the appropriate function with the
 * absolute value of n it is called with.
 *
 * @param p The point to move.
 * @param n The number of places to move.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int cursor_point_move_cols_backward(point *p, uint n);
static int cursor_point_move_cols_forward(point *p, uint n);

/*
 * Move a point a number of lines either forward or backward.
 * cursor_point_move_lines wraps these, calling the appropriate function with
 * the absolute value of n it is called with.
 *
 * @param p The point to move.
 * @param n The number of places to move.
 *
 * @return  0 on success, -1 on error.
 *
 */
static int cursor_point_move_lines_backward(point *p, uint n);
static int cursor_point_move_lines_forward(point *p, uint n);

/*
 * Wrapped by cursor_point_sub. Returns the result of subtracting a smaller
 * point from a larger one. Unlike cursor_point_sub, the larger point must be
 * the first argument.
 *
 * @param larger  The larger of the two points to subtract.
 * @param smaller The smaller of the two points to subtract.
 *
 * @return        The difference between the points.
 *
 */
static long cursor_point_sub_abs(point *larger, point *smaller);

/*
 * Handers to move lineno->point references when lines are deleted, inserted
 * or changed in a buffer. All are called only by hooks.
 *
 */
static void cursor_point_handle_line_delete(vec *args, hook h);
static void cursor_point_handle_line_insert(vec *args, hook h);
static void cursor_point_handle_line_change(vec *args, hook h);

hook_add(cursor_point_on_move_pre,  1);
hook_add(cursor_point_on_move_post, 3);

int cursor_point_initsys(void)
{
    /* Initialize the table of all points. Keys are pointers, so *
     * we do not need null values.                               */
    TRACE_PTR(table_create(&cursor_point_all,
                            sizeof(table), sizeof(buffer *),
                            NULL, NULL, NULL),
              return -1);

    /* Mount the various functions required to keep track of changes *
     * to this table. (Insertions are made in cursor_point_init)     */
    TRACE_INT(hook_mount(&buffer_line_on_delete_post,
                          cursor_point_handle_line_delete, 600),
              return -1);

    TRACE_INT(hook_mount(&buffer_line_on_insert_post,
                          cursor_point_handle_line_insert, 600),
              return -1);

    TRACE_INT(hook_mount(&buffer_line_on_change_post,
                          cursor_point_handle_line_change, 600),
              return -1);

    return 0;
}

static size_t point_invalid_index = INVALID_INDEX;

point *cursor_point_init(buffer *b)
{
    table *subtable;
    vec   *subvec;
    point *rtn;

    ASSERT_PTR(b, high, return NULL);
    ASSERT_PTR(rtn = malloc(sizeof(point)),
               terminal, return NULL);

    rtn->ln = 0;
    rtn->cn = 0;
    rtn->b  = b;

    /* Look for a table in cursor_point_all pertaining to this buffer */
    subtable = table_get(&cursor_point_all, &b);

    /* If there is no such subtable, we gotta make our own. */
    if (subtable == NULL)
    {
        /* This is how we init things in-place in tables.               *
         * - Insert value *NULL at key (This sets no memory)            *
         * - Get a pointer to this undefined memory                     *
         * - Use table_create to init a table at this location in-place */
        TRACE_INT(table_set(&cursor_point_all, &b, NULL),
                  free(rtn);
                  return NULL);
        TRACE_PTR(subtable = table_get(&cursor_point_all, &b),
                  free(rtn);
                  return NULL);
        TRACE_PTR(table_create(subtable, sizeof(vec), sizeof(lineno),
                               NULL, NULL, (void *)&point_invalid_index),
                  free(rtn);
                  return NULL);
    }

    /* See if there is a vector pertaining to the linenumber of this point */
    subvec = table_get(subtable, &(rtn->ln));

    /* If there is one. We gotta make one (yay) */
    if (subvec == NULL)
    {
        /* Init a vec in-place in the table. *
         * See above for details.            */
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

    /* Insert into the vector of points in this buffer and at this lineno, *
     * the new point.                                                      */

    TRACE_INT(vec_insert_end(subvec, 1, &rtn),
              free(rtn);
              return NULL);

    return rtn;
}

static int cursor_point_move_ln(point *p, lineno ln)
{
    table *subtable;
    vec   *subvec;

    /* May as well save time on the 1% */
    if (ln == p->ln)
        return 0;

    /* Find this point's vector */
    ASSERT_PTR(subtable = table_get(&cursor_point_all, &(p->b)),
               critical, return -1);
    ASSERT_PTR(subvec   = table_get(subtable, &(p->ln)),
               critical, return -1);

    /* Delete this point from its current vector. */
    ASSERT_INT(vec_delete(subvec, vec_find(subvec, &p), 1),
               critical, return -1);

    /* If necessary, remove the vector that contained this point */
    if (vec_len(subvec) == 0)
    {
        vec_kill(subvec);
        ASSERT_INT(table_delete(subtable, &(p->ln)),
                   critical, return -1);
    }

    /* Find or create a new vector for this point */
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

    /* Insert it into its new home */
    ASSERT_INT(vec_insert_end(subvec, 1, &p),
               critical, return -1);

    /* Tell it where it lives now */
    p->ln = ln;

    /* Ensure that it hasn't gone off the end of a line */
    TRACE_INT(cursor_point_fix_cn(p),
              return -1);

    return 0;
}

int cursor_point_free(point *p)
{
    table *subtable;
    vec   *subvec;

    ASSERT_PTR(subtable = table_get(&cursor_point_all, &(p->b)),
               critical, return -1);
    ASSERT_PTR(subvec   = table_get(subtable, &(p->ln)),
               critical, return -1);

    /* Delete this point from its current vector. */
    ASSERT_INT(vec_delete(subvec, vec_find(subvec, &p), 1),
               critical, return -1);

    /* If necessary, remove the vector that contained this point */
    if (vec_len(subvec) == 0)
    {
        vec_kill(subvec);
        ASSERT_INT(table_delete(subtable, &(p->ln)),
                   critical, return -1);

        /* And if that was necessary, maybe we need to remove the table *
         * containing that vector as well...                            */
        if (table_len(subtable) == 0)
        {
            table_kill(subtable);
            ASSERT_INT(table_delete(&cursor_point_all, &(p->b)),
                       critical, return -1);
        }
    }

    return 0;
}

static int cursor_point_fix_cn(point *p)
{
    size_t linelen;

    linelen = buffer_len_line(p->b, p->ln);

    if (p->cn > linelen)
        p->cn = linelen;

    return 0;
}

static int cursor_point_fix_ln(point *p)
{
    size_t buflen;

    buflen = buffer_len(p->b);

    if (buflen == 0)
        TRACE_INT(cursor_point_move_ln(p, 0),
                  return -1);
    else if (p->ln >= buflen)
        TRACE_INT(cursor_point_move_ln(p, buflen - 1),
                  return -1);

    return 0;
}

static int cursor_point_move_cols_backward(point *p, uint n)
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

    TRACE_INT(cursor_point_move_ln(p, ln),
              return -1);

    return 0;
}

static int cursor_point_move_cols_forward(point *p, uint n)
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

    TRACE_INT(cursor_point_move_ln(p, ln),
              return -1);

    return 0;
}

int cursor_point_move_cols(point *p, int n)
{
    int    rtn;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(cursor_point_on_move_pre, p);

    if (n < 0)
        rtn = cursor_point_move_cols_backward(p, (uint)-n);

    else if (n > 0)
        rtn = cursor_point_move_cols_forward(p, (uint)n);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    return rtn;
}

static int cursor_point_move_lines_forward(point *p, uint n)
{
    size_t buflen;

    buflen = buffer_len(p->b);

    if (n + p->ln >= buflen)
        TRACE_INT(cursor_point_move_ln(p, buflen - 1),
                  return -1);
    else
        TRACE_INT(cursor_point_move_ln(p, p->ln + n),
                  return -1);

    TRACE_INT(cursor_point_fix_cn(p),
              return -1);

    return 0;
}

static int cursor_point_move_lines_backward(point *p, uint n)
{
    if (n >= p->ln)
        TRACE_INT(cursor_point_move_ln(p, 0),
                  return -1);
    else
        TRACE_INT(cursor_point_move_ln(p, p->ln - n),
                  return -1);

    return 0;
}

int cursor_point_move_lines(point *p, int n)
{
    int rtn;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    hook_call(cursor_point_on_move_pre, p);

    if (n < 0)
        rtn = cursor_point_move_lines_backward(p, (uint)-n);
    else if (n > 0)
        rtn = cursor_point_move_lines_forward(p, (uint)n);

    cursor_point_fix_cn(p);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    return rtn;
}

#define DELETE_BATCH_THRESHOLD 40

int cursor_point_delete(point *p, uint n)
{
    vec   *origline, *newline;
    colno  origcn;
    lineno origln;

    TRACE_PTR(origline = buffer_get_line(p->b, p->ln),
              return -1);

    origcn = p->cn;
    origln = p->ln;

    hook_call(cursor_point_on_move_pre, p);

    if (n >= DELETE_BATCH_THRESHOLD)
        buffer_batch_start(p->b);

    while (n > p->cn && p->ln)
    {
        TRACE_INT(cursor_point_move_ln(p, p->ln - 1),
                  vec_free(origline);
                  buffer_batch_end(p->b);
                  return -1);

        n -= (uint)p->cn + 1;

        TRACE_INT(buffer_delete(p->b, p->ln + 1),
                  vec_free(origline);
                  buffer_batch_end(p->b);
                  return -1);

        p->cn = buffer_len_line(p->b, p->ln);
    }

    TRACE_PTR(newline = buffer_get_line(p->b, p->ln),
              vec_free(origline);
              buffer_batch_end(p->b);
              return -1);

    n = MIN(n, (uint)p->cn);

    TRACE_INT(vec_delete(newline, p->cn - n, vec_len(newline) - p->cn + n),
              vec_free(origline);
              buffer_batch_end(p->b);
              return -1);

    if (origcn < vec_len(origline))
        TRACE_INT(vec_insert_end(newline, vec_len(origline) - origcn,
                                 vec_item(origline, origcn)),
                  vec_free(origline);
                  vec_free(newline);
                  buffer_batch_end(p->b);
                  return -1);


    p->cn -= n;

    TRACE_INT(buffer_set_line(p->b, p->ln, newline),
              vec_free(origline);
              vec_free(newline);
              buffer_batch_end(p->b);
              return -1);

    buffer_batch_end(p->b);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    vec_free(origline);
    vec_free(newline);

    return 0;
}

int cursor_point_insert(point *p, const char *str)
{
    size_t inslen;
    vec   *l;
    lineno origln;
    colno  origcn;

    origln = p->ln;
    origcn = p->cn;

    if (buffer_len(p->b) == 0)
        TRACE_INT(buffer_insert(p->b, p->ln),
                  return -1);

    inslen = strlen(str);

    TRACE_PTR(l = buffer_get_line(p->b, p->ln),
              return -1);

    hook_call(cursor_point_on_move_pre, p);

    TRACE_INT(vec_insert(l, p->cn, inslen, str),
              return -1);

    p->cn += inslen;

    TRACE_INT(buffer_set_line(p->b, p->ln, l),
              return -1);

    vec_free(l);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    return 0;
}

int cursor_point_enter(point *p)
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

    hook_call(cursor_point_on_move_pre, p);

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

    TRACE_INT(cursor_point_move_ln(p, newln),
              vec_free(start);
              vec_free(end);
              return -1);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    vec_free(start);
    vec_free(end);

    return 0;
}

lineno cursor_point_get_ln(point *p)
{
    ASSERT_PTR(p, high, return INVALID_INDEX);

    return p->ln;
}

int cursor_point_set_ln(point *p, lineno ln)
{
    lineno origln;
    colno  origcn;

    ASSERT_PTR(p, high, return -1);

    origln = p->ln;
    origcn = p->cn;

    hook_call(cursor_point_on_move_pre, p);

    TRACE_INT(cursor_point_move_ln(p, ln),
              return -1);

    TRACE_INT(cursor_point_fix_ln(p),
              return -1);
    TRACE_INT(cursor_point_fix_cn(p),
              return -1);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    return 0;
}

colno cursor_point_get_cn(point *p)
{
    ASSERT_PTR(p, high, return INVALID_INDEX);

    return p->cn;
}

int cursor_point_set_cn(point *p, lineno cn)
{
    lineno origln;
    colno  origcn;

    ASSERT_PTR(p, high, return -1);

    origln = p->ln;
    origcn = p->cn;

    hook_call(cursor_point_on_move_pre, p);

    p->cn = cn;

    TRACE_INT(cursor_point_fix_cn(p),
              return -1);

    hook_call(cursor_point_on_move_post, p, &origln, &origcn);

    return 0;
}

int cursor_point_cmp(point *a, point *b)
{
    ASSERT    (a || b, high, return 0);
    ASSERT_PTR(a,      high, return -1);
    ASSERT_PTR(b,      high, return  1);

    if (a->ln == b->ln)
    {
        if (a->cn == b->cn)
            return 0;

        return (a->cn > b->cn) ? 1 : -1;
    }

    return (a->ln > b->ln) ? 1 : -1;
}

static long cursor_point_sub_abs(point *larger, point *smaller)
{
    lineno  ln;
    long    sum;
    buffer *b;

    b   = larger->b;
    sum = 0;

    /* Sum up the line lengths of all lines including the line the *
     * smaller point is on, till the one just before the line the  *
     * larger point is on.                                         */
    for (ln = smaller->ln; ln < larger->ln; ln++)
        sum += (long)buffer_len_line(b, ln) + 1;

    /* Subtract the column number of the smaller point. Since we *
     * already added the length of the line it's on, subtracting *
     * its column number will include the portion of the line    *
     * after the point in the sum.                               */
    sum -= (long)smaller->cn;
    /* Add the column number of the larger point. */
    sum += (long)larger->cn;

    return sum;
}

long cursor_point_sub(point *a, point *b)
{
    int cmp;

    ASSERT_PTR(a, high, return 0);
    ASSERT_PTR(b, high, return 0);

    ASSERT(a->b == b->b, high, return 0);

    cmp = cursor_point_cmp(a, b);

    /* a > b */
    if (cmp == 1)
        return cursor_point_sub_abs(a, b);
    /* a < b */
    else if (cmp == -1)
        return cursor_point_sub_abs(b, a);
    /* a = b */
    else
        return 0;
}

static void cursor_point_handle_line_delete(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table  *subtable;
    vec    *tomove;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&cursor_point_all, &b);

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

                hook_call(cursor_point_on_move_pre, p);

                cursor_point_move_ln(p, p->ln - 1);
                cursor_point_fix_ln(p);

                hook_call(cursor_point_on_move_post, p, &origln, &origcn);
        );

    vec_free(tomove);
}

static void cursor_point_handle_line_change(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table *subtable;
    vec   *subvec;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&cursor_point_all, &b);

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

                hook_call(cursor_point_on_move_pre, p);

                cursor_point_fix_cn(p);

                hook_call(cursor_point_on_move_post, p, &origln, &origcn);

        );
}

static void cursor_point_handle_line_insert(vec *args, hook h)
{
    buffer *b;
    lineno  ln;
    table  *subtable;
    vec    *tomove;

    b  =  *(buffer **)vec_item(args, 0);
    ln = **(lineno **)vec_item(args, 1);

    subtable = table_get(&cursor_point_all, &b);

    if (subtable == NULL)
        return;

    tomove = vec_init(sizeof(point *));

    table_foreach_ptr(subtable, vec *, v,
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

                hook_call(cursor_point_on_move_pre, p);

                if (p->ln + 1 < buffer_len(p->b))
                    cursor_point_move_ln(p, p->ln + 1);

                hook_call(cursor_point_on_move_post, p, &origln, &origcn);
        );

    vec_free(tomove);
}
