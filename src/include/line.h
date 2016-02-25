#ifndef LINE_H
#define LINE_H
#include <stddef.h>
#include <stdint.h>
#include "vec.h"

typedef size_t lineno;
typedef size_t colno;

typedef struct textcont_s textcont;

typedef struct chunk_s chunk;

typedef struct line_s line;

struct textcont_s
{
	chunk *currchunk;
};

void line_free(line *l);

int line_delete(line *l);

int line_set_text(line *l, char *c);

char *line_get_text(line *l);

lineno line_get_lineno(line *l);

lineno line_get_lineno_hint(line *l, lineno hline, chunk *hchunk);

line *textcont_insert  (textcont *t, lineno n);

line *textcont_get_line(textcont *t, lineno n);

lineno textcont_get_total_lines(textcont *t);

size_t textcont_get_total_chars(textcont *t);

#endif /* LINE_H */
