#include "line.h"
#include "vec.h"
#include "err.h"
#include "face.h"

#include <stdlib.h>
#include <string.h>

struct line_s
{
    chunk *chunk;
    vec   *faces;
    colno  length;
    char  *data;
};

void lines_set_chunk(vec *lines, chunk *newchunk);

void lines_set_chunk(vec *lines, chunk *newchunk)
{
    vec_foreach(lines, line *, l,
                l->chunk = newchunk);
}


line *line_init(chunk *c)
{
    line *rtn;

    rtn = malloc(sizeof(line));

    CHECK_ALLOC(line_init, rtn, NULL);

    rtn->faces  = vec_init(sizeof(face));

    if (rtn->faces == NULL)
    {
        free(rtn);
        TRACE(line_init, vec_init(sizeof(face)), NULL);
    }

    rtn->chunk  = c;
    rtn->length = 0;
    rtn->data   = NULL;

    return rtn;
}

void line_free(line *l)
{
    if (l->chunk)
        line_delete(l);

    vec_free(l->faces);
    free(l->data);
    free(l);
}

lineno line_get_len(line *l)
{
    if (l == NULL)
        return 0;

    return l->length;
}

int line_delete(line *l)
{
    CHECK_NULL_PRM(line_delete, l, -1);
    TRACE_NONZRO_CALL(line_delete, chunk_remove_item(l->chunk, l), -1);

    return 0;
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

int line_set_text(line *l, const char *c)
{
    size_t len;

    CHECK_NULL_PRM(line_set_text, l, -1);
    CHECK_NULL_PRM(line_set_text, c, -1);

    len = strlen(c);

    l->data    = realloc(l->data, len);

    CHECK_ALLOC(line_set_text, l->data, -1);

    memcpy(l->data, c, len + 1);

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

const char *line_get_text_const(line *l)
{
    CHECK_NULL_PRM(line_get_text, l, NULL);

    return l->data;
}

lineno line_get_lineno(line *l)
{
    lineno start, offset;

    CHECK_NULL_PRM(line_get_lineno, l, INVALID_INDEX);

    start  = chunk_get_startindex(l->chunk);
    offset = chunk_find_item_offset(l->chunk, l);

    if (start == INVALID_INDEX)
        TRACE(line_get_lineno, chunk_get_startindex(l->chunk), INVALID_INDEX);

    if (offset == INVALID_INDEX)
        TRACE(line_get_lineno, chunk_find_item_offset(l->chunk, l), INVALID_INDEX);

    return start + offset;
}

lineno line_get_lineno_hint(line *l, lineno hln, chunk *hchunk)
{
    size_t dev, clen, hoffset, maxdev, offset;
    chunk *c;

    CHECK_NULL_PRM(line_get_lineno_hint, l,               INVALID_INDEX);
    CHECK_NULL_PRM(line_get_lineno_hint, l->chunk,        INVALID_INDEX);

    c = l->chunk;
    hoffset = hln - chunk_get_startindex(c);

    if (c == hchunk)
    {
        if ((line*)chunk_get_item(c, hln) == l)
            return hln;

        dev  = 0;
        clen = chunk_len(c);

        maxdev = hoffset > clen - hoffset ? hoffset : clen - hoffset;
        while (dev < maxdev)
        {
            ++dev;

            if (dev + hoffset < clen)
                if ((line*)chunk_get_item(c, hoffset + dev) == l)
                    return hln + dev;

            if (dev <= clen)
                if ((line*)chunk_get_item(c, hoffset - dev) == l)
                    return hln - dev;
        }

        ERR_NEW(critical, "line_get_lineno_hint: Line not found",
                "The line thinks it is in a chunk of text that does not contain it (by deviation search)");
        return INVALID_INDEX;
    }
    else if (chunk_next(c) == hchunk)
        offset = chunk_rfind_item_offset(c, l);

    else
        offset = chunk_find_item_offset(c, l);

    if (offset == INVALID_INDEX)
    {
        ERR_NEW(critical, "line_get_lineno_hint: Line not found",
                "The line thinks it is in a chunk of text that does not contain it");
        return INVALID_INDEX;
    }

    return chunk_get_startindex(c) + offset;
}

chunk *line_get_chunk(line *l)
{
    CHECK_NULL_PRM(line_get_chunk, l, NULL);

    return l->chunk;
}

vec *line_get_faces(line *l)
{
    CHECK_NULL_PRM(line_get_faces, l, NULL);

    return l->faces;
}
