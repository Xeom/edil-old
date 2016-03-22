#ifndef LINE_H
#define LINE_H

#include <stddef.h>
#include <stdint.h>

#include "container/vec.h"
#include "chunk.h"

typedef size_t lineno;
typedef size_t colno;

typedef struct line_s line;

line  *line_init(chunk *chunk);

void line_free(line *l);

lineno line_get_len(line *l);

int line_delete(line *l);

int line_insert_text(line *l, colno pos, char *c);

int line_delete_text(line *l, colno pos, colno n);

int line_set_text(line *l, const char *c);

char *line_get_text(line *l);

const char *line_get_text_const(line *l);

lineno line_get_lineno(line *l);

lineno line_get_lineno_hint(line *l, lineno hline, chunk *hchunk);

chunk *line_get_chunk(line *l);

vec *line_get_faces(line *l);

#endif /* LINE_H */
