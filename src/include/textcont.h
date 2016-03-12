#ifndef TEXTCONT_H
# define TEXTCONT_H
#include "line.h"

typedef struct textcont_s textcont;


struct textcont_s
{
    chunk *currchunk;
};

textcont *textcont_init(void);

line *textcont_insert(textcont *t, lineno n);

line *textcont_get_line(textcont *t, lineno n);

lineno textcont_get_total_lines(textcont *t);

int textcont_is_last_line(textcont *t, line *l);

int textcont_is_first_line(textcont *t, line *l);

int textcont_has_line(textcont *t, lineno ln);

#endif /* TEXTCONT_H */
