#include "line.h"
#include "vec.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>

struct line_s
{
    chunk *chunk;
    colno  length;
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

chunk *chunk_init(void);
void   chunk_free(chunk *c);
int    chunk_resize(chunk *c);

void   reset_chunk_starts(chunk *c);
void   lines_move_chunk(vec *lines, chunk *newchunk);

line  *line_init(chunk *chunk);

chunk *chunk_init(void)
{
    chunk *rtn = malloc(sizeof(chunk));

    CHECK_ALLOC(chunk_init, rtn, NULL);

    rtn->startline = 0;
    rtn->next      = NULL;
    rtn->prev      = NULL;
    rtn->lines     = vec_init(sizeof(line*));

    if (rtn->lines == NULL)
    {
        free(rtn);
        TRACE(chunk_init, vec_init(sizeof(line*)), NULL);
    }

    return rtn;
}

textcont *textcont_init(void)
{
    textcont *rtn = malloc(sizeof(textcont));

    CHECK_ALLOC(textcont_init, rtn, NULL);

    rtn->currchunk = chunk_init();

    if (rtn->currchunk == NULL)
    {
        free(rtn);
        TRACE(textcont_init, chunk_init(), NULL);
    }

    return rtn;
}

void chunk_free(chunk *c)
{
    vec_free(c->lines);
    free(c);
}

line *line_init(chunk *c)
{
    line *rtn;

    rtn = malloc(sizeof(line));

    CHECK_ALLOC(line_init, rtn, NULL);

    rtn->chunk  = c;
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

lineno line_get_len(line *l)
{
    if (l == NULL)
        return 0;

    return l->length;
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
            ERR_NEW(critical, "chunk_resize: Could not vec_slice lines up",
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
                ERR_NEW(critical, "chunk_resize: Could not init new chunk",
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
            ERR_NEW(critical, "chunk_resize: Could not append lines to dump",
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
            ERR_NEW(critical, "chunk_resize: Could not append lines to dump",
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
    CHECK_NULL_PRM(line_delete, l,        -1);
    CHECK_NULL_PRM(line_delete, l->chunk, -1);

    if (vec_remove(l->chunk->lines, &l))
    {
        ERR_NEW(critical, line_delete: Removing line from vector failed, vec_err_str());
        return -1;
    }

    if (chunk_resize(l->chunk))
    {
        ERR_NEW(critical, line_delete: Resizing chunk failed, NULL);
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

    CHECK_NULL_PRM(textcont_insert, t, NULL);

    c = t->currchunk;

    while (c->startline > n)
    {
        if (c->prev == NULL)
        {
            ERR_NEW(critical, textcont_insert: First chunk startline nonzero,
                    Chunk with no prev link has a startline greater than requested line);
            return NULL;
        }
        c = c->prev;
    }

    while (c->startline + vec_len(c->lines) <= n)
    {
        if (c->next == NULL)
        {
            ERR_NEW(medium, textcont_insert: Line out of range of textcont, NULL);
            return NULL;
        }
        c = c->next;
    }

    t->currchunk = c;

    offset = n - t->currchunk->startline;

    rtn = line_init(c);

    if (rtn == NULL)
    {
        ERR_NEW(critical, textcont_insert: Could not create new line,
                line_init returned NULL);

        return NULL;
    }

    if(vec_insert(c->lines, offset, &rtn))
    {
        if (vecerr == E_VEC_INVALID_INDEX)
            ERR_NEW(high, textcont_insert: Invalid index handed to function, NULL);

        else
            ERR_NEW(critical, textcont_insert: Inserting into lines vector failed, vec_err_str());

        return NULL;
    }

    if (chunk_resize(c))
    {
        ERR_NEW(critical, textcont_insert: Could not resize chunk, NULL);
        return NULL;
    }

    reset_chunk_starts(c);

    return rtn;
}

line *textcont_get_line(textcont *t, lineno n)
{
    size_t  offset;
    chunk  *c;

    CHECK_NULL_PRM(textcont_get_line, t, NULL);

    c = t->currchunk;

    while (c->startline > n)
    {
        if (c->prev == NULL)
        {
            ERR_NEW(critical, textcont_get_line: First chunk startline nonzero,
                    Chunk with no prev link has a startline greater than requested line);
            return NULL;
        }

        c = c->prev;
    }

    while (c->startline + vec_len(c->lines) <= n)
    {
        if (c->next == NULL)
        {
            ERR_NEW(medium, textcont_get_line: Line out of range of textcont, NULL);
            return NULL;
        }
        c = c->next;
    }

    t->currchunk = c;

    offset = n - c->startline;

    return *(line**)vec_get(c->lines, offset);
}

int line_insert_text(line *l, colno pos, char *c)
{
    size_t len;

    CHECK_NULL_PRM(line_insert_text, l, -1);
    CHECK_NULL_PRM(line_insert_text, c, -1);

    len = strlen(c);

    l->length += len;
    l->data    = realloc(l->data, l->length);

    CHECK_ALLOC(line_insert_text, l->data, -1);

    if (pos > l->length)
        pos = l->length;

    memmove(l->data + pos + len,
            l->data + pos,  len);

    memmove(l->data + pos, c, len);

    return 0;
}

int line_delete_text(line *l, colno pos, colno n)
{
    CHECK_NULL_PRM(line_delete_text, l, -1);

    if (pos > l->length)
        pos = l->length;

    if (n > l->length - pos)
        n = l->length - pos;

    memmove(l->data + pos,
            l->data + pos + n, n);

    l->length -= n;
    l->data    = realloc(l->data, l->length);

    CHECK_ALLOC(line_delete_text, l->data, -1);

    return 0;
}


int line_set_text(line *l, char *c)
{
    size_t len;

    CHECK_NULL_PRM(line_set_text, l, -1);
    CHECK_NULL_PRM(line_set_text, c, -1);

    len = strlen(c);
    l->data    = realloc(l->data, len);

    CHECK_ALLOC(line_set_text, l->data, -1);

    memcpy(l->data, c, len);

    return 0;
}

char *line_get_text(line *l)
{
    char *rtn;

    CHECK_NULL_PRM(line_get_text, l, NULL);

    rtn = malloc(l->length + 1);

    CHECK_ALLOC(line_set_text, rtn, NULL);

    memcpy(rtn, l->data, l->length);
    rtn[l->length] = '\0';

    return rtn;
}

lineno line_get_lineno(line *l)
{
    chunk *c;
    vec   *v;

    CHECK_NULL_PRM(line_get_lineno, l, INVALID_INDEX);

    c = l->chunk;

    if (c == NULL)
        return INVALID_INDEX;

    v = c->lines;

    if (v == NULL)
    {
        ERR_NEW(critical, line_get_lineno: NULL chunk lines vector,
                chunk * -> lines was NULL);
        return INVALID_INDEX;
    }

    return c->startline + vec_find(v, &l);
}

lineno line_get_lineno_hint(line *l, lineno hline, chunk *hchunk)
{
    size_t index, deviation;
    chunk *c;
    vec   *v;

    CHECK_NULL_PRM(line_get_lineno_hint, l,               INVALID_INDEX);
    CHECK_NULL_PRM(line_get_lineno_hint, l->chunk,        INVALID_INDEX);
    CHECK_NULL_PRM(line_get_lineno_hint, l->chunk->lines, INVALID_INDEX);

    c = l->chunk;
    v = c->lines;

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

        ERR_NEW(critical, line_get_lineno_hint: Line not found,
                The line thinks it is in a chunk of text that does not contain it (by deviation search));
        return INVALID_INDEX;
    }
    else if (l->chunk->prev == hchunk)
    {
        index = vec_rfind(v, &l);

        if (index == INVALID_INDEX)
        {
            ERR_NEW(critical, line_get_lineno_hint: Line not found,
                    The line thinks it is in a chunk of text that does not contain it (by vec_rfind));
            return INVALID_INDEX;
        }

        return l->chunk->startline + index;
    }

    index = vec_find(v, &l);

    if (index == INVALID_INDEX)
    {
        ERR_NEW(critical, line_get_lineno_hint: Line not found,
                The line thinks it is in a chunk of text that does not contain it (by vec_find));
        return INVALID_INDEX;
    }

    return l->chunk->startline + index;
}

chunk *line_get_chunk(line *l)
{
    CHECK_NULL_PRM(line_get_chunk, l, NULL);

    return l->chunk;
}

lineno textcont_get_total_lines(textcont *t)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_get_total_lines, t, INVALID_INDEX);

    c = t->currchunk;

    if (c == NULL)
        return INVALID_INDEX;

    while (c->next)
        c = c->next;

    return vec_len(c->lines) + c->startline;
}

size_t textcont_get_total_chars(textcont *t)
{
    int    offset;
    size_t count;
    chunk *chunk;

    CHECK_NULL_PRM(textcont_get_total_lines, t, INVALID_INDEX);

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

    while (chunk && chunk->prev)
    {
        chunk = chunk->prev;

        offset = vec_len(chunk->lines);

        while (offset--)
            count += (*((line**)vec_get(chunk->lines, offset)))->length;
    }

    return count;
}

int textcont_has_line(textcont *t, lineno ln)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_last_line, t, -1);

    c = t->currchunk;

    while (c)
    {
        if (c->startline + vec_len(c->lines) > ln)
            return 1;

        c = c->next;
    }

    return 0;
}

int textcont_is_last_line(textcont *t, line *l)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_last_line, t,        -1);
    CHECK_NULL_PRM(textcont_is_last_line, l,        -1);
    CHECK_NULL_PRM(textcont_is_last_line, l->chunk, -1);

    c = l->chunk;

    if (c->next)
        return 0;

    if (*((line**)vec_get(c->lines, vec_len(c->lines))) ==  l)
        return 1;

    return 0;
}

int textcont_is_first_line(textcont *t, line *l)
{
    chunk *c;

    CHECK_NULL_PRM(textcont_is_first_line, t,        -1);
    CHECK_NULL_PRM(textcont_is_first_line, l,        -1);
    CHECK_NULL_PRM(textcont_is_first_line, l->chunk, -1);

    c = l->chunk;

    if (c->prev)
        return 0;

    if (*((line**)vec_get(c->lines, 0)) == l)
        return 1;

    return 0;
}
