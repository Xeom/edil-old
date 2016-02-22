#include "line.h"
#include "vec.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>

struct line_s
{
	chunk *chunk;
	size_t length;
	char  *data;
};

#define CHUNK_DEFAULT  128

#define CHUNK_MIN      (128 << 2)
#define CHUNK_MAX      (128 + (128 << 2))

struct chunk_s
{
	lineno startline;
	chunk *next;
	chunk *prev;
	vec   *lines;
};

chunk *chunk_init(void)
{
	chunk *rtn = malloc(sizeof(chunk));

	if (rtn == NULL)
	{
		err_new(terminal, "chunk_init: Out of memory",
				"Could not allocate memory for new chunk");
		return NULL;
	}

	rtn->startline = 0;
	rtn->next      = NULL;
	rtn->prev      = NULL;
	rtn->lines     = vec_init(sizeof(line*));

	return rtn;
}

void chunk_free(chunk *c)
{
	vec_free(c->lines);
	free(c);
}

line *line_init(chunk *chunk)
{
	line *rtn;

	rtn = malloc(sizeof(line));

	if (rtn == NULL)
	{
		err_new(terminal, "line_init: Out of memory",
				"Could not allocate memory for new line of text");
		return NULL;
	}

	rtn->chunk  = chunk;
	rtn->length = 0;
	rtn->data   = NULL;

	return rtn;
}

void line_free(line *l)
{
	if (l->chunk)
		line_delete(l);

	free(l->data);
	free(l);
}

void reset_chunk_starts(chunk *c)
{
	while (c->next)
	{
		lineno newstart = c->startline + vec_len(c->lines);
		c->next->startline = newstart;
		c = c->next;
	}
}

void lines_move_chunk(vec *lines, chunk *newchunk)
{
	size_t i = 0;
	while (i < vec_len(lines))
		(*(line**)vec_get(lines, i++))->chunk = newchunk;
}

int chunk_resize(chunk *c)
{
	chunk *dump;
	vec   *overflow;

	if (vec_len(c->lines) > CHUNK_MAX)
	{
		overflow = vec_slice(c->lines, CHUNK_DEFAULT, vec_len(c->lines));

		if (overflow == NULL)
		{
			err_new(critical, "chunk_resize: Could not vec_slice lines up",
					vec_err_str());

			return -1;
		}

		if (c->next)
		{
			dump = c->next;
		}
		else
		{
			dump = chunk_init();

			if (dump == NULL)
			{
				err_new(critical, "chunk_resize: Could not init new chunk",
						"Could not initialize a new chunk to dump overflow lines in");
				return -1;
			}

			dump->prev = c;
			c->next    = dump;
		}

		lines_move_chunk(overflow, dump);

		if(vec_append(dump->lines, overflow))
		{
			vec_free(overflow);
			err_new(critical, "chunk_resize: Could not append lines to dump",
					vec_err_str());
			return -1;
		}

		vec_free(overflow);

		return chunk_resize(dump);

	}

	if (vec_len(c->lines) < CHUNK_MIN && c->next)
	{
		dump   = c->next;
		overflow = c->lines;

		c->next->prev = c->prev;
		if (c->prev)
			c->prev->next = c->next;

		lines_move_chunk(overflow, dump);

		if (vec_append(dump->lines, overflow))
		{
			err_new(critical, "chunk_resize: Could not append lines to dump",
					vec_err_str());
			chunk_free(c);
			return -1;
		}

		chunk_free(c);

		return chunk_resize(dump);
	}

	return 0;
}

int line_delete(line *l)
{
	if (l->chunk == NULL)
	{
		err_new(high, "line_delete: Handed NULL line", NULL);
		return -1;
	}

	if (vec_remove(l->chunk->lines, &l))
	{
		err_new(critical, "line_delete: Removing line from vector failed", vec_err_str());
		return -1;
	}

	if (chunk_resize(l->chunk))
	{
		err_new(critical, "line_delete: Resizing chunk failed", NULL);
		return -1;
	}

	reset_chunk_starts(l->chunk);

	l->chunk = NULL;

	return 0;
}

line *textcont_insert(textcont *t, lineno n)
{
	size_t  offset;
	line   *rtn;
	chunk  *c;

	if (t == NULL)
	{
		err_new(high, "textcont_insert: Handed NULL textcontainer", NULL);
		return NULL;
	}

	c = t->currchunk;

	while (c->startline > n)
	{
		if (c->prev == NULL)
		{
			err_new(critical, "textcont_insert: First chunk startline nonzero",
					"Chunk with no prev link has a startline greater than requested line");
			return NULL;
		}
		c = c->prev;
	}

	while (c->startline + vec_len(c->lines) <= n)
	{
		if (c->next == NULL)
		{
			err_new(medium, "textcont_insert: Line out of range of textcont", NULL);
			return NULL;
		}
		c = c->next;
	}

	t->currchunk = c;

	offset = n - t->currchunk->startline;

	rtn = line_init(c);

	if (rtn == NULL)
	{
		err_new(critical, "textcont_insert: Could not create new line",
				"line_init returned NULL");

		return NULL;
	}

	if(vec_insert(c->lines, offset, &rtn))
	{
		if (vecerr == E_VEC_INVALID_INDEX)
			err_new(high, "textcont_insert: Invalid index handed to function", NULL);

		else
			err_new(critical, "textcont_insert: Inserting into lines vector failed", vec_err_str());

		return NULL;
	}

	if (chunk_resize(c))
	{
		err_new(critical, "textcont_insert: Could not resize chunk", NULL);
		return NULL;
	}

	reset_chunk_starts(c);

	return rtn;
}

line *textcont_get_line(textcont *t, lineno n)
{
	size_t  offset;
	chunk  *c;

	if (t == NULL)
	{
		err_new(high, "textcont_get_line: Handed NULL textcontainer", NULL);
		return NULL;
	}

	c = t->currchunk;

	while (c->startline > n)
	{
		if (c->prev == NULL)
		{
			err_new(critical, "textcont_get_line: First chunk startline nonzero",
					"Chunk with no prev link has a startline greater than requested line");
			return NULL;
		}

		c = c->prev;
	}

	while (c->startline + vec_len(c->lines) <= n)
	{
		if (c->next == NULL)
		{
			err_new(medium, "textcont_get_line: Line out of range of textcont", NULL);
			return NULL;
		}
		c = c->next;
	}

	t->currchunk = c;

	offset = n - c->startline;

	return *(line**)vec_get(c->lines, offset);
}

int line_set_text(line *l, char *c)
{
	size_t len;

	if (l == NULL)
	{
		err_new(high, "line_set_text: Handed null line", NULL);
		return -1;
	}

	if (c == NULL)
	{
		err_new(high, "line_set_text: Handed null string", NULL);
		return -1;
	}

	len = strlen(c);
	l->data    = realloc(l->data, len);

	if (l->data == NULL)
		err_new(terminal, "line_set_text: Out of memory",
				"Could not realloc memory for new text data of line");
	else
		memcpy(l->data, c, len);

	return 0;
}

char *line_get_text(line *l)
{
	char *rtn;

	if (l == NULL)
	{
		err_new(high, "line_get_text: Handed null line", NULL);
		return NULL;
	}

	rtn = malloc(l->length);

	if (rtn == NULL)
	{
		err_new(terminal, "line_get_text: Out of memory",
				"Could not allocate memory for strinng to return line data");
		return NULL;
	}

	memcpy(rtn, l->data, l->length);

	return rtn;
}

lineno line_get_lineno(line *l)
{
	chunk *c;
	vec   *v;

	if (l == NULL)
	{
		err_new(high, "line_get_lineno: Handed null line", NULL);
		return INVALID_LINE;
	}

	c = l->chunk;

	if (c == NULL)
		return INVALID_LINE;

	v = c->lines;

	if (v == NULL)
	{
		err_new(critical, "line_get_lineno: NULL chunk lines vector",
				"chunk * -> lines was NULL");
		return INVALID_LINE;
	}

	return c->startline + vec_find(v, &l);
}

lineno line_get_lineno_hint(line *l, lineno hline, chunk *hchunk)
{
	size_t index, deviation;
	chunk *c;
	vec   *v;

	c = l->chunk;

	if (c == NULL)
		return INVALID_LINE;

	v = c->lines;

	if (v == NULL)
	{
		err_new(critical, "line_get_lineno_hint: NULL chunk lines vector",
				"chunk * -> lines was NULL");
		return INVALID_LINE;
	}

	if (c == hchunk)
	{
		if (*(line**)vec_get(v, hline) == l)
			return hline;

		deviation = 0;
		while (deviation <= hline &&
			   deviation  + hline < vec_len(v))
		{
			++deviation;

			if (deviation + hline < vec_len(v))
				if (*(line**)vec_get(v, hline + deviation) == l)
					return hline + deviation;

			if (deviation <= hline)
				if (*(line**)vec_get(hchunk->lines, hline - deviation) == l)
					return hline + deviation;
		}

		err_new(critical, "line_get_lineno_hint: Line not found",
				"The line thinks it is in a chunk of text that does not contain it (by deviation search)");
		return INVALID_LINE;
	}
	else if (l->chunk->prev == hchunk)
	{
		index = vec_rfind(v, &l);

		if (index == INVALID_INDEX)
		{
			err_new(critical, "line_get_lineno_hint: Line not found",
					"The line thinks it is in a chunk of text that does not contain it (by vec_rfind)");
			return INVALID_LINE;
		}

		return l->chunk->startline + index;
	}

	index = vec_find(v, &l);

	if (index == INVALID_INDEX)
	{
		err_new(critical, "line_get_lineno_hint: Line not found",
				"The line thinks it is in a chunk of text that does not contain it (by vec_find)");
		return INVALID_LINE;
	}

	return l->chunk->startline + index;
}

lineno textcont_get_total_lines(textcont *t)
{
	chunk *c;

	if (t == NULL)
		return INVALID_LINE;

	c = t->currchunk;

	if (c == NULL)
		return INVALID_LINE;

	while (c->next)
		c = c->next;

	return vec_len(c->lines) + c->startline;
}

size_t textcont_get_total_chars(textcont *t)
{
	int    offset;
	size_t count;
	chunk *chunk;

	count = 0;
	chunk = t->currchunk;

	while (chunk)
	{
		offset = vec_len(chunk->lines);

		while (offset--)
			count += (*((line**)vec_get(chunk->lines, offset)))->length;

		chunk = chunk->next;
	}

	chunk = t->currchunk;

	while (chunk->prev)
	{
		chunk = chunk->prev;

		offset = vec_len(chunk->lines);

		while (offset--)
			count += (*((line**)vec_get(chunk->lines, offset)))->length;
	}

	return count;
}
