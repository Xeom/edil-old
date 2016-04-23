#if !defined(BUFFER_CORE_H)
# define BUFFER_CORE_H
# include "head.h"

# include "container/table.h"
# include "container/vec.h"

# include "hook.h"

# include "buffer/line.h"


/* Note that for buffer_line_on*_pre functions, an operation can be disabled by enabling the        *
 * readonly flag of the buffer. This does not, of course, work for *_post functions.                */

/* These hooks are all called with two parameters, a buffer and a     *
 * linenumber. They are called before (_pre) and after (_post) a line *
 * is deleted from a buffer.                                          */
extern hook buffer_line_on_delete_pre;
extern hook buffer_line_on_delete_post;

/* These hooks are all called ith two parameters, a buffer and a     *
 * linenumber. They are called before (_pre) and after (_post) a new *
 * line is inserted into a buffer.                                   */
extern hook buffer_line_on_insert_pre;
extern hook buffer_line_on_insert_post;

/* These hooks are called with three parameters, a buffer, a         *
 * linenumber, and a vector of content they are called before (_pre) *
 * and after (_post) the contents of a line are changed. The vector  *
 * argument is the new contents of the line.                         */
extern hook buffer_line_on_change_pre;
extern hook buffer_line_on_change_post;

/* These hooks are all called with one parameter, a buffer */
extern hook buffer_on_create; /* This hook is called when a new buffer is created */
extern hook buffer_on_delete; /* This hook is called before a buffer is deleted   */

extern uint buffer_readonly_flag;
extern uint buffer_modified_flag;

typedef struct buffer_s buffer;

/*
 * Initialize and return a new buffer.
 *
 */
buffer *buffer_init(void);

/*
 * Free a buffer, it's chunks and lines.
 *
 * b is the buffer to free.
 *
 */
void buffer_free(buffer *b);

/*
 * Create and insert a new line into a buffer at a particular line number.
 * Subsequent lines are shifted accordingly.
 *
 * b is the buffer to insert into.
 *
 * ln is the linenumber of the new line.
 *
 */
int buffer_insert(buffer *b, lineno ln);

/*
 * Delete a specific line from a buffer. Subsequent lines are shifted 
 * accordingly.
 *
 * b is the buffer to delete from.
 *
 * ln is the linenumber of the line to delete.
 *
 */
int buffer_delete(buffer *b, lineno ln);

/*
 * Get a vector containing the contents of a particular line. The vector will
 * not be linked to the line in any way, and must be freed by the caller of this
 * function.
 *
 * b is the buffer to get a line from.
 *
 * ln is the linenumber of the line whose contents we want.
 *
 */
vec *buffer_get_line(buffer *b, lineno ln);

/*
 * Set the contents of a line to contents provided in a vector.
 *
 * b is the buffer to set a line in.
 *
 * ln is the linenumber of the line to set contents of.
 *
 */
int buffer_set_line(buffer *b, lineno ln, vec *l);

/*
 * Get the value (0 or 1) of a particular flag in a buffer.
 *
 * b is the buffer to find the value of a flag for.
 *
 * flag is the value of the buffer_*_flag variable that relates
 * to the flag we want to get.
 *
 */
int buffer_get_flag(buffer *b, uint flag);

/*
 * Disable a flag (set it to 0) of a particular buffer.
 *
 * b is the buffer to disable a flag for.
 *
 * flag is the value of the buffer_*_flag variable that relates
 * to the flag we want to disable.
 *
 */
int buffer_disable_flag(buffer *b, uint flag);

/*
 * Enable a flag (set it to 0) of a particular buffer.
 *
 * b is the buffer to disable a flag for.
 *
 * flag is the value of the buffer_*_flag variable that relates
 * to the flag we want to enable.
 *
 */
int buffer_enable_flag(buffer *b, uint flag);

/*
 * Get number of lines in a buffer.
 *
 * b is the buffer to count lines in.
 *
 */
lineno buffer_len(buffer *b);

table *buffer_get_properties(buffer *b);

#endif /* BUFFER_CORE_H */
