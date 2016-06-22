#if !defined(CURSOR_REGION_H)
# define CURSOR_REGION_H
# include "head.h"

# include "cursor/cursor.h"
# include "buffer/buffer.h"

cursor_type cursor_region_type;

typedef struct region_s region;

int cursor_region_initsys(void);

region *cursor_region_init(buffer *b);

int cursor_region_free(region *r);

lineno cursor_region_get_ln(region *r);
colno  cursor_region_get_cn(region *r);

int cursor_region_set_ln(region *r, lineno ln);
int cursor_region_set_cn(region *r, colno cn);

int cursor_region_move_cols(region *r, int n);
int cursor_region_move_lines(region *r, int n);

int cursor_region_insert(region *r, const char *str);

int cursor_region_delete(region *r, uint n);

int cursor_region_enter(region *r);

int cursor_region_activate(region *r);
int cursor_region_deactivate(region *r);

#endif /* CURSOR_REGION_H */
