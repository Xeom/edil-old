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

textcont *textcont_init(void);

void line_free(line *l);

lineno line_get_len(line *l);

int line_delete(line *l);

line *textcont_insert(textcont *t, lineno n);

line *textcont_get_line(textcont *t, lineno n);

int line_insert_text(line *l, colno pos, char *c);

int line_delete_text(line *l, colno pos, colno n);

int line_set_text(line *l, char *c);

char *line_get_text(line *l);

lineno line_get_lineno(line *l);

lineno line_get_lineno_hint(line *l, lineno hline, chunk *hchunk);

chunk *line_get_chunk(line *l);

vec *line_get_faces(line *l);

lineno textcont_get_total_lines(textcont *t);

size_t textcont_get_total_chars(textcont *t);

int textcont_is_last_line(textcont *t, line *l);

int textcont_is_first_line(textcont *t, line *l);

int textcont_has_line(textcont *t, lineno ln);

#endif /* LINE_H */
