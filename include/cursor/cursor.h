#if !defined(CURSOR_CURSOR_H)
# define CURSOR_CURSOR_H
# include "head.h"

# include "buffer/buffer.h"

typedef struct cursor_type_s cursor_type;
typedef struct cursor_s      cursor;

struct cursor_type_s
{
    void *(*init) (buffer *);
    int   (*free) (void *);

    lineno  (*get_ln)     (void *); //
    colno   (*get_cn)     (void *); //
    buffer *(*get_buffer) (void *);

    int (*set_ln) (void *, lineno); //
    int (*set_cn) (void *, colno);  //

    int (*move_cols)  (void *, int);
    int (*move_lines) (void *, int);

    int (*insert) (void *, char *);
    int (*delete) (void *, uint);

    int (*activate)   (void *);
    int (*deactivate) (void *);
};

int cursor_initsys(void);

cursor *cursor_spawn(buffer *b, cursor_type *type);

lineno cursor_get_ln(cursor *cur);
colno cursor_get_cn(cursor *cur);
buffer *cursor_get_buffer(cursor *cur);
int cursor_set_ln(cursor *cur, lineno ln);
int cursor_set_cn(cursor *cur, colno cn);
int cursor_move_cols(cursor *cur, int n);
int cursor_move_lines(cursor *cur, int n);
int cursor_insert(cursor *cur, char *str);
int cursor_delete(cursor *cur, uint n);

cursor *cursor_buffer_selected(buffer *b);

vec *cursor_buffer_all(buffer *b);

cursor *cursor_current(void);

#endif /* CURSOR_CURSOR_H */
