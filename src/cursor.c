#include "cursor.h"
#include "err.h"
#include <string.h>
#include <stdlib.h>

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

	next = cursor_get_lineno(cur) + 1;

	rtn = textcont_get_line(cur->text, next);
	TRACE_NULL(cursor_get_next_line, textcont_get_line(cur->text, next), rtn, NULL);

	return rtn;
}

line *cursor_prev_line(cursor *cur)
{
	lineno prev, curr;
	line *rtn;

	curr = cursor_get_lineno(cur);

	if (curr == 0)
	{
		ERR_NEW(medium, cursor_prev_line: Start of textcont,
				Could not go back a line because we are already at the first one);
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

	i = 0;

	while (1)
	{
		linelen = line_get_len(currline);
		linetext = line_get_text(currline);

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

			return rtn;
		}

		if (islast == -1)
		{
			ERR_NEW(err_last_lvl,
					cursor_get_text_forwards: Call "textcont_is_first_line(cur->text, currline)" failed,
					textcont_is_first_line(cur->text, currline) returned -1);
			return NULL;
		}

		rtn[i] = '\n';

		if (i == n)
			return rtn;

		--currln;
		currline = textcont_get_line(cur->text, currln);
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

	i = n;
	rtn[i] = '\0';

	while (1)
	{
		linelen  = line_get_len(currline);
		linetext = line_get_text(currline);

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
			rtn[n - i] = '\0';

			return rtn;
		}

		if (isfirst == -1)
		{
			ERR_NEW(err_last_lvl,
					cursor_get_text_backwards: Call "textcont_is_first_line(cur->text, currline)" failed,
					"textcont_is_first_line(cur->text, currline)" returned -1);
			return NULL;
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
	colno col;

	col = cursor_get_col(cur);
	t   = cur->text;
	l   = cur->l;

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
		{
			ERR_NEW(err_last_lvl, cursor_delete_backwards: Call "textcont_is_first_line(t, l)" failed,
					"textcont_is_first_line(t, l)" returned -1);
			return -1;
		}

		prev = cursor_prev_line(cur);

		linetext = line_get_text(prev);
		line_insert_text(l, 0, linetext);
		free(linetext);

		col += line_get_len(prev);

		line_delete(prev);
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
		{
			ERR_NEW(err_last_lvl, cursor_delete_forwards: Call textcont_is_last_line(t, l) failed,
					textcont_is_last_line(t, l) returned -1);
			return -1;
		}

		next = cursor_next_line(cur);

		linetext = line_get_text(next);
		line_insert_text(l, line_get_len(l), linetext);
		free(linetext);

		line_delete(next);
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
