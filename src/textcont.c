#include "textcont.h"
#include "line.h"
#include "vec.h"
#include "err.h"

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

    while (c->startline + vec_len(c->lines) < n)
    {
        if (c->next == NULL)
        {
            ERR_NEW(medium, textcont_insert: Line out of range of textcont, \0);
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
