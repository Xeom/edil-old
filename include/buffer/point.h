#if !defined(BUFFER_POINT_H)
# define BUFFER_POINT_H
# include "head.h"

# include "buffer/line.h"

typedef struct point_s point;

int buffer_point_initsys(void);

point *buffer_point_init(buffer *b, lineno ln, colno cn);

int buffer_point_move_cols(point *p, int n);

int buffer_point_move_lines(point *p, int n);

int buffer_point_delete(point *p, uint n);

int buffer_point_insert(point *p, char *str);

int buffer_point_enter(point *p);

#endif /* BUFFER_POINT_H */
