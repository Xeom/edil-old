#ifndef CURSOR_H
#define CURSOR_H
#include "line.h"

typedef struct cursor_s cursor;

lineno cursor_get_lineno(cursor *cur);

colno cursor_get_col(cursor *cur);

int cursor_set_col(cursor *cur, colno n);

int cursor_set_lineno(cursor *cur, lineno n);

int cursor_insert(cursor *cur, char *str);

char *cursor_get_text_forwards(cursor *cur, size_t n);

char *cursor_get_text_backwards(cursor *cur, size_t n);

int cursor_delete_backwards(cursor *cur, size_t n);

int cursor_delete_forwards(cursor *cur, size_t n);

int cursor_goto_line_end(cursor *cur);

#endif /* CURSOR_H */
