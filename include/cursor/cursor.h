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

    int (*set_ln) (void *, lineno); //
    int (*set_cn) (void *, colno);  //

    int (*move_lines) (void *, int);
    int (*move_cols)  (void *, int);

    int (*insert) (void *, const char *);
    int (*delete) (void *, uint);
    int (*enter)  (void *);

    int (*activate)   (void *);
    int (*deactivate) (void *);
};

extern hook cursor_on_spawn;
extern hook cursor_on_set_ln_pre;
extern hook cursor_on_set_ln_post;
extern hook cursor_on_set_cn_pre;
extern hook cursor_on_set_cn_post;
extern hook cursor_on_move_lines_pre;
extern hook cursor_on_move_lines_post;
extern hook cursor_on_move_cols_pre;
extern hook cursor_on_move_cols_post;
extern hook cursor_on_insert_pre;
extern hook cursor_on_insert_post;
extern hook cursor_on_delete_pre;
extern hook cursor_on_delete_post;
extern hook cursor_on_enter_pre;
extern hook cursor_on_enter_post;
extern hook cursor_on_change_pos;

int cursor_initsys(void);

cursor *cursor_spawn(buffer *b, cursor_type *type);

int cursor_free(cursor *cur);

buffer *cursor_get_buffer(cursor *cur);

void *cursor_get_ptr(cursor *cur);
cursor_type *cursor_get_type(cursor *cur);
lineno cursor_get_ln (cursor *cur);
colno cursor_get_cn  (cursor *cur);
int cursor_set_ln    (cursor *cur, lineno ln);
int cursor_set_cn    (cursor *cur, colno cn);
int cursor_move_cols (cursor *cur, int n);
int cursor_move_lines(cursor *cur, int n);
int cursor_insert    (cursor *cur, const char *str);
int cursor_delete    (cursor *cur, uint n);
int cursor_enter     (cursor *cur);
int cursor_activate  (cursor *cur);
int cursor_deactivate(cursor *cur);

cursor *cursor_buffer_selected(buffer *b);

vec *cursor_buffer_all(buffer *b);

cursor *cursor_selected(void);

int cursor_select_last(buffer *b);

int cursor_select_next(buffer *b);

#endif /* CURSOR_CURSOR_H */
