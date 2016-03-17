#include <string.h>
#include <stdlib.h>

#include "err.h"
#include "textcont.h"

#include "cursor.h"

struct cursor_s
{
    colno     col;
    line     *l;
    lineno    probablyline;
    chunk    *probablychunk;
    textcont *text;
};

void cursor_set_heuristics(cursor *cur, lineno n);
line *cursor_next_line(cursor *cur);
line *cursor_prev_line(cursor *cur);

void cursor_set_heuristics(cursor *cur, lineno n)
{
    cur->probablyline = n;
    cur->probablychunk = line_get_chunk(cur->l);
}

lineno cursor_get_lineno(cursor *cur)
{
    lineno n;

    CHECK_NULL_PRM(cursor_get_lineno, cur,    INVALID_INDEX);

    n = line_get_lineno_hint(cur->l,
                             cur->probablyline,
                             cur->probablychunk);

    if (n == INVALID_INDEX)
        TRACE(cursor_get_lineno, line_get_lineno_hint(cur->l, cur->probablyline, cur->probablychunk), INVALID_INDEX);

    cursor_set_heuristics(cur, n);

    return n;
}

colno cursor_get_col(cursor *cur)
{
    colno len;

    CHECK_NULL_PRM(cursor_get_col, cur,    INVALID_INDEX);
    CHECK_NULL_PRM(cursor_get_col, cur->l, INVALID_INDEX);

    len = line_get_len(cur->l);

    if (cur->col > len)
        return len;
    else
        return cur->col;

    return 0;
}

int cursor_set_col(cursor *cur, colno n)
{
    colno len;

    CHECK_NULL_PRM(cursor_set_col, cur,    -1);
    CHECK_NULL_PRM(cursor_set_col, cur->l, -1);

    len = line_get_len(cur->l);

    if (n > len)
        cur->col = len;
    else
        cur->col = n;

    return 0;
}

int cursor_set_lineno(cursor *cur, lineno n)
{
    line *l;

    CHECK_NULL_PRM(cursor_set_line, cur, -1);

    l = textcont_get_line(cur->text, n);

    TRACE_NULL(cursor_set_line, textcont_get_line(cur->text, n), l, -1);

    cur->l = l;
    cursor_set_heuristics(cur, n);

    return 0;
}

int cursor_insert(cursor *cur, char *str)
{
    TRACE_NONZRO_CALL(cursor_insert, line_insert_text(cur->l, cur->col, str),                -1);
    TRACE_NONZRO_CALL(cursor_insert, cursor_set_col(cur, cursor_get_col(cur) + strlen(str)), -1);

    return 0;
}


line *cursor_next_line(cursor *cur)
{
    lineno next;
    line  *rtn;

    next = cursor_get_lineno(cur);

    if (next == INVALID_INDEX)
        TRACE(cursor_next_line, cursor_get_lineno(cur), NULL);

    next += 1;

    rtn = textcont_get_line(cur->text, next);
    TRACE_NULL(cursor_next_line, textcont_get_line(cur->text, next), rtn, NULL);

    return rtn;
}

line *cursor_prev_line(cursor *cur)
{
    lineno prev, curr;
    line *rtn;

    curr = cursor_get_lineno(cur);

    if (curr == INVALID_INDEX)
        TRACE(cursor_prev_line, cursor_get_lineno(cur), NULL);

    if (curr == 0)
    {
        ERR_NEW(medium, "cursor_prev_line: Start of textcont",
                "Could not go back a line because we are already at the first one");
        return NULL;
    }

    prev = curr - 1;

    rtn = textcont_get_line(cur->text, prev);
    TRACE_NULL(cursor_get_next_line, textcont_get_line(cur->text, prev), rtn, NULL);

    return rtn;
}

char *cursor_get_text_forwards(cursor *cur, size_t n)
{
    char  *rtn, *linetext;
    line  *currline;
    colno  linelen;
    lineno currln;
    int    islast;
    size_t i;

    rtn      = malloc(n + 1);
    currln   = cursor_get_lineno(cur);
    currline = cur->l;

    CHECK_ALLOC(cursor_get_text_forwards, rtn, NULL);

    if (currln == INVALID_INDEX)
    {
        free(rtn);
        TRACE(cursor_get_text_forwards, cursor_get_lineno(cur), NULL);
    }

    i = 0;

    while (1)
    {
        linelen = line_get_len(currline);
        linetext = line_get_text(currline);

        if (linelen == INVALID_INDEX)
        {
            free(rtn);
            TRACE(cursor_get_text_forwards, line_get_len(currline), NULL);
        }

        if (linetext == NULL)
        {
            free(rtn);
            TRACE(cursor_get_text_forwards, line_get_text(currline), NULL);
        }

        if (linelen < n - i)
        {
            memcpy(rtn + i, linetext, linelen);
            i += linelen;
        }
        else
        {
            memcpy(rtn + i, linetext, n - i);
            return rtn;
        }

        islast = textcont_is_first_line(cur->text, currline);

        if (islast == 1)
        {
            rtn = realloc(rtn, i + 1);
            rtn[i] = '\0';

            CHECK_ALLOC(cursor_get_text_forwards, rtn, NULL);

            return rtn;
        }

        if (islast == -1)
        {
            free(rtn);
            TRACE(cursor_get_text_forwards, textcont_is_first_line(cur->text, currline), NULL);
        }

        rtn[i] = '\n';/* TODO: support \r\n, \v, \f etc */

        if (i == n)
            return rtn;

        --currln;
        currline = textcont_get_line(cur->text, currln);

        if (currline == NULL)
        {
            free(rtn);
            TRACE(cursor_get_text_forwards, textcont_get_line(cur->text, currln), NULL);
        }
    }
}

char *cursor_get_text_backwards(cursor *cur, size_t n)
{
    char  *rtn, *linetext;
    line  *currline;
    int    isfirst;
    colno  linelen;
    lineno currln;
    size_t i;

    rtn      = malloc(n + 1);
    currln   = cursor_get_lineno(cur);
    currline = cur->l;

    CHECK_ALLOC(cursor_get_text_backwards, rtn, NULL);

    if (currln == INVALID_INDEX)
    {
        free(rtn);
        TRACE(cursor_get_text_backwards, cursor_get_lineno(cur), NULL);
    }

    i = n;
    rtn[i] = '\0';

    while (1)
    {
        linelen  = line_get_len(currline);
        linetext = line_get_text(currline);

        if (linelen == INVALID_INDEX)
        {
            free(rtn);
            TRACE(cursor_get_text_backwards, line_get_len(currline), NULL);
        }

        if (linetext == NULL)
        {
            free(rtn);
            TRACE(cursor_get_text_backwards, line_get_text(currline), NULL);
        }

        if (linelen < i)
        {
            i -= linelen;
            memcpy(rtn + i, linetext, linelen);
        }
        else
        {
            memcpy(rtn, linetext + linelen - i, linelen);
            return rtn;
        }

        isfirst = textcont_is_first_line(cur->text, currline);

        if (isfirst == 1)
        {
            memmove(rtn, rtn + i, n - i);
            rtn = realloc(rtn, n - i + 1);

            CHECK_ALLOC(cursor_get_text_backwards, rtn, NULL);

            rtn[n - i] = '\0';

            return rtn;
        }

        if (isfirst == -1)
        {
            free(rtn);
            TRACE(cursor_get_text_backwards, textcont_is_first_line(cur->text, currline), NULL);
        }

        --i;
        rtn[i] = '\n';

        if (i == 0)
            return rtn;

        --currln;
        currline = textcont_get_line(cur->text, currln);
    }
}

int cursor_delete_backwards(cursor *cur, size_t n)
{
    char *linetext;
    line *l, *prev;
    textcont *t;
    int isfirst;
    colno col, prevlen;

    CHECK_NULL_PRM(cursor_delete_backwards, cur, -1);

    col = cursor_get_col(cur);

    t   = cur->text;
    l   = cur->l;

    if (col == INVALID_INDEX)
        TRACE(cursor_delete_backwards, cursor_get_col(cur), -1);

    while (n > col)
    {
        n--;

        isfirst = textcont_is_first_line(t, l);

        if (isfirst == 1)
        {
            n = col;
            break;
        }

        if (isfirst == -1)
            TRACE(cursor_delete_backwards, textcont_is_first_line(t, l), -1);

        prev = cursor_prev_line(cur);

        TRACE_NULL(cursor_delete_backwards, cursor_prev_line(cur), prev, -1);

        linetext = line_get_text(prev);

        TRACE_NULL(cursor_delete_backwards, line_get_text(prev), linetext, -1);
        TRACE_NONZRO_CALL(cursor_delete_backwards, line_insert_text(l, 0, linetext), -1);

        free(linetext);

        prevlen= line_get_len(prev);

        if (prevlen == INVALID_INDEX)
            TRACE(cursor_delete_backwards, line_get_len(prev), -1);

        col += prevlen;

        TRACE_NONZRO_CALL(cursor_delete_backwards, line_delete(prev), -1);
        line_free(prev); /* ACTUALLY DO KILLBUFFER STUFF */
    }

    TRACE_NONZRO_CALL(cursor_delete_backwards, line_delete_text(cur->l, col - n, n), -1);
    TRACE_NONZRO_CALL(cursor_delete_backwards, cursor_set_col(cur, col - n),         -1);

    return 0;
}

int cursor_delete_forwards(cursor *cur, size_t n)
{
    char *linetext;
    colno     col;
    line     *l, *next;
    textcont *t;
    int       islast;

    col = cursor_get_col(cur);

    if (col == INVALID_INDEX)
        TRACE(cursor_delete_forwards, cursor_get_col(cur), -1);

    t   = cur->text;
    l   = cur->l;

    while (n > line_get_len(l) - col)
    {
        n--;

        islast = textcont_is_last_line(t, l);

        if (islast == 1)
        {
            n = line_get_len(l) - col;
            break;
        }

        if (islast == -1)
            TRACE(cursor_delete_forwards, textcont_is_last_line(t, l), -1);

        next = cursor_next_line(cur);

        TRACE_NULL(cursor_delete_forwards, cursor_next_line(cur), next, -1);

        linetext = line_get_text(next);

        TRACE_NULL(cursor_delete_forwards, line_get_text(next), linetext, -1);

        TRACE_NONZRO_CALL(cursor_delete_forwards, line_insert_text(l, line_get_len(l), linetext), -1);
        free(linetext);

        TRACE_NONZRO_CALL(cursor_delete_forwards, line_delete(next), -1);
        line_free(next); /* ACTUALLY DO KILLBUFFER STUFF */
    }

    TRACE_NONZRO_CALL(cursor_delete_forwards, line_delete_text(l, col, n), -1);

    return 0;
}

int cursor_goto_line_end(cursor *cur)
{/* TODO: Errors */
    cur->col = line_get_len(cur->l);

    return 0;
}
