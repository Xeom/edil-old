#include "cursor.h"
#include "err.h"
#include <string.h>

struct cursor_s
{
	colno     col;
	line     *l;
	lineno    probablyline;
	chunk    *probablychunk;
	textcont *text;
};

void cursor_set_heuristics(cursor *cur, lineno n)
{
	cur->probablyline = n;
	cur->probablychunk = line_get_chunk(cur->l);
}

lineno cursor_get_lineno(cursor *cur)
{
	lineno n;

	n = line_get_lineno_hint(cur->l,
							 cur->probablyline,
							 cur->probablychunk);

	cursor_set_heuristics(cur, n);

	return n;
}

colno cursor_get_col(cursor *cur)
{
	colno len;

	CHECK_NULL_PRM(cursor_set_col, cur, -1);

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

	CHECK_NULL_PRM(cursor_set_col, cur, -1);

	len = line_get_len(cur->l);

	if (n > len)
		cur->col = len;
	else
		cur->col = n;

	return 0;
}

int cursor_set_line(cursor *cur, lineno n)
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

int cursor_delete_backwards(cursor *cur, size_t n)
{/* MAKE THIS PRETTY ALSO */
	colno col;

	col = cursor_get_col(cur);

	if (col < n)
		n = col;

	TRACE_NONZRO_CALL(cursor_delete_backwards, line_delete_text(cur->l, col - n, n), -1);
	TRACE_NONZRO_CALL(cursor_delete_backwards, cursor_set_col(cur, col - n),         -1);

	return 0;
}

line *cursor_next_line(cursor *cur)
{
	lineno next;
	line  *rtn;

	next = cursor_get_lineno(cur) + 1;

	rtn = textcont_get_line(cur->text, next);
	TRACE_NULL(cursor_get_next_line, textcont_get_line(cur->text, next), rtn, NULL);

	return rtn;
}

int cursor_delete_forwards(cursor *cur, size_t n)
{
	colno     col, deln;
	line     *l, *next;
	textcont *t;
	int       islast;

	col = cursor_get_col(cur);
	t   = cur->text;
	l   = cur->l;

	while (n > line_get_len(l) - col)
	{
		n--;

		islast = textcont_is_last_line(t, l);

		if (islast == 1)
			break;

		if (islast == -1)
		{
			err_new(err_last_lvl, "cursor_delete_forwards: Call textcont_is_last_line(t, l) failed",
					"textcont_is_last_line(t, l) returned -1");
			return -1;
		}

		next = cursor_next_line(cur);

		line_insert_text(l, line_get_len(l), line_get_text(next));

		line_delete(next);
		line_free(next);
	}

	deln = line_get_len(l);
	deln = deln > n ? n : deln;

	TRACE_NONZRO_CALL(cursor_delete_forwards, line_delete_text(l, col, deln), -1);

	return 0;
}

int cursor_goto_line_end(cursor *cur)
{/* TODO: Errors */
	cur->col = line_get_len(cur->l);

	return 0;
}
