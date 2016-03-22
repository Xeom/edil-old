#include <stdlib.h>
#include <string.h>

#include "container/vec.h"
#include "err.h"
#include "face.h"

#include "line.h"

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

    ASSERT_PTR(rtn = malloc(sizeof(line)), terminal,
               return NULL);

    TRACE_PTR(rtn->faces  = vec_init(sizeof(face)),
              free(rtn);
              return NULL);

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
    ASSERT_PTR(l, high,
               return -1);

    TRACE_INT(chunk_remove_item(l->chunk, l),
              return -1);

    return 0;
}

int line_insert_text(line *l, colno pos, char *c)
{
    size_t len;

    ASSERT_PTR(l, high, return -1);
    ASSERT_PTR(c, high, return -1);

    len = strlen(c);

    l->length += len;

    ASSERT_PTR(l->data = realloc(l->data, l->length), terminal,
               return -1);

    if (pos > l->length)
        pos = l->length;

    memmove(l->data + pos + len,
            l->data + pos,  len);

    memmove(l->data + pos, c, len);

    return 0;
}

int line_delete_text(line *l, colno pos, colno n)
{
    ASSERT_PTR(l, high, return -1);

    if (pos > l->length)
        pos = l->length;

    if (n > l->length - pos)
        n = l->length - pos;

    memmove(l->data + pos,
            l->data + pos + n, n);

    l->length -= n;

    ASSERT_PTR(l->data = realloc(l->data, l->length), terminal,
               return -1);

    return 0;
}

int line_set_text(line *l, const char *c)
{
    size_t len;

    ASSERT_PTR(l, high, return -1);
    ASSERT_PTR(c, high, return -1);

    len = strlen(c);

    ASSERT_PTR(l->data = realloc(l->data, len), terminal,
               return -1);

    memcpy(l->data, c, len + 1);

    return 0;
}

char *line_get_text(line *l)
{
    char *rtn;

    ASSERT_PTR(l, high, return NULL);

    ASSERT_PTR(rtn = malloc(l->length + 1), terminal,
               return NULL);

    memcpy(rtn, l->data, l->length);

    rtn[l->length] = '\0';

    return rtn;
}

const char *line_get_text_const(line *l)
{
    ASSERT_PTR(l, high, return NULL);

    return l->data;
}


chunk *line_get_chunk(line *l)
{
    ASSERT_PTR(l, high, return NULL);

    return l->chunk;
}

vec *line_get_faces(line *l)
{
    ASSERT_PTR(l, high, return NULL);

    return l->faces;
}

lineno line_get_lineno(line *l)
{
    lineno start, offset;

    ASSERT_PTR(l, high, return INVALID_INDEX);

    TRACE_IND(start = chunk_get_startindex(l->chunk),
              return INVALID_INDEX);

    TRACE_IND(offset = chunk_find_item_offset(l->chunk, l),
              return INVALID_INDEX);

    return start + offset;
}

lineno line_get_lineno_hint(line *l, lineno hln, chunk *hchunk)
{
    size_t dev, clen, hoffset, maxdev, offset;
    chunk *c;

    ASSERT_PTR(l,        high, return INVALID_INDEX);
    ASSERT_PTR(l->chunk, high, return INVALID_INDEX);

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

