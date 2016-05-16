#if !defined(BUFFER_POINT_H)
# define BUFFER_POINT_H
# include "head.h"

# include "buffer/line.h"

typedef struct point_s point;

/*
 */
extern hook buffer_point_on_move_pre;
extern hook buffer_point_on_move_post;

/* A point is basically a cursor. We do not call it a cursor since cursor     *
 * refers to a variety of objects that can be moved around and edit text. A   *
 * point is only one kind of cursor. Cursors could implement rectangular      *
 * selection, overtype, drawing borders etc. A point however is a very simple *
 * type of cursor.                                                            *
 *                                                                            *
 * The cursor system keeps track of every cursor and adjusts their positions  *
 * appropriately when the content of their buffer changes. This is in         *
 * contrast to every cursor keeping a reference to the line object it is in.  *
 * Considering the relatively low number of cursors in existance at any one   *
 * time, it was deemed more appropriate to use linenumbers to manage points.  */

/*
 * Initialize the point system.
 *
 * @return 0 on success, -1 on error.
 *
 */
int buffer_point_initsys(void);

/*
 * Initialize and return a new point in a specific buffer, at a specific
 * position. If the desired position does not exist, it is corrected to
 * the closest possible position.
 *
 * @param b  A pointer to the buffer to initialize the new point in.
 * @param ln The line number to initialize the new point at.
 * @param cn The column number to initialize the new point at.
 *
 * @return   A pointer to the new point, or NULL on error.
 *
 */
point *buffer_point_init(buffer *b, lineno ln, colno cn);

/*
 * Move a point a certain number of lines or columns. Positive numbers
 * correspond to down and right, while negative numbers correspond to up and
 * left. Attempts to move the point to a position that does not exist will move
 * it to the closest possible position. Moving the point to the right of a line
 * will wrap it to the next line. Moving it farther than the length of the next
 * line also will wrap it again, and so on.
 *
 * @param p A pointer to the point to move.
 * @param n The number of places to move the point (can be negative)
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_point_move_cols(point *p, int n);
int buffer_point_move_lines(point *p, int n);

/*
 * Delete a certain number of characters behind a point. When a break between
 * two lines is included in the range to be deleted, it is counted as one
 * character and the remainder of the later line is moved onto the end of the
 * first one. The point's position is moved so that it is between the last and 
 * first characters not deleted, before and after the deleted region.
 *
 * @param p A pointer to the point to delete characters with.
 * @param n The number of characters to delete.
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_point_delete(point *p, uint n);

/*
 * Insert a string at a point into a buffer. The string is inserted starting
 * at the current position of the point, and the point is then moved after the
 * string. The string provided to this function should be null terminated.
 * Newlines in the string are treated as normal characters and inserted into the
 * buffer. Calls to buffer_point_enter or equivilent should be made to insert
 * new lines into a buffer with a point.
 *
 * @param p   A point to the pointer to insert a string at.
 * @param str A pointer to a string of characters to insert into the buffer.
 *
 * @return    0 on success, -1 on error.
 *
 */
int buffer_point_insert(point *p, char *str);

/*
 * Insert a new line at a point's location. If the point is midway through a
 * line, move the content of the line after the point onto the new line. The
 * point is moved to the start of the new line.
 *
 * @param p The point to insert a new line at.
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_point_enter(point *p);

/*
 */
lineno buffer_point_get_ln(point *p);

/*
 */
void   buffer_point_set_ln(point *p, lineno ln);

/*
 */
colno buffer_point_get_cn(point *p);

/*
 */
void  buffer_point_set_cn(point *p, colno cn);

/*
 */
buffer *buffer_point_get_buffer(point *p);

/*
 */
int  buffer_point_cmp(point *a, point *b);

/*
 */
long buffer_point_sub(point *a, point *b);

#endif /* BUFFER_POINT_H */
