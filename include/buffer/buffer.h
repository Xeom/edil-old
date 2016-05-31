#if !defined(BUFFER_BUFFER_H)
# define BUFFER_BUFFER_H
# include "head.h"

# include "container/table.h"
# include "container/vec.h"

# include "hook.h"

# include "buffer/line.h"

/* Note that for buffer_line_on*_pre functions, an operation can be   *
 * aborted by enabling the __READONLY property of the buffer. This    *
 * does not, of course, work for *_post functions.                    */

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

/* This hook is called when a new buffer is created */
extern hook buffer_on_create;
/* This hook is called before a buffer is deleted */
extern hook buffer_on_delete;


/* This hook is called with three parameters, a buffer, and two      *
 * linenumbers, a start and an end. It is called whenever            *
 * buffer_batch_end is successfully called. The content in between   *
 * and including the two linenumbers can be anything.                */
extern hook buffer_on_batch_region;

typedef struct buffer_s buffer;

/*
 * Initialize and return a new buffer.
 *
 * Testable in unit_buffer.test_init
 *
 * @return A pointer to the new buffer
 *
 */
buffer *buffer_init(void);

/*
 * Free a buffer, it's chunks and lines.
 *
 * Testable by valgrind in unit_buffer.test_init
 *
 * @param b A pointer to the buffer to free.
 *
 */
void buffer_free(buffer *b);

/*
 * This function is used to enable batch mode of a buffer. When in batch mode,
 * the range of lines between which any changes were made to the buffer is
 * stored. Batch mode can be used to get performance increases when doing a lot
 * of operations on a region of a buffer. Batch mode should be enabled before
 * and disabled after the operations. This only invoes one hook call, to
 * buffer_on_batch_region, rather than individual pre and post calls to specific
 * hooks. This means that various applications may break if batch mode is used,
 * as they rely on the other hooks, meaning batch mode is best used for special
 * cases, like loading a file.
 *
 * @param b A pointer to the buffer to enable batch mode on.
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_batch_start(buffer *b);

/*
 * See buffer_batch_start.
 * Disables batch mode of a buffer. Calls the buffer_on_batch_region hook with
 * the linenumbers between which all changes took place.
 *
 * @param b A pointer to the buffer to disable batch mode on.
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_batch_end(buffer *b);

/* Possibly a bad idea ... line *buffer_get_line_struct(buffer *b, lineno ln);*/

/*
 * Get the table of properties from a buffer. The table will be indexed with
 * strings, so table_get/table_set should be called with a char** as keys.
 * Values are pointers, but are assumed to be strings, though nothing in the
 * buffer system relies on this. Higher level systems may well rely on this, so
 * values should be set to valid char**s.
 *
 * @param b A pointer to the buffer to get the table of properties from.
 *
 * @return  A pointer to the table of properties for this particular buffer,
 *          NULL on error.
 *
 */
table *buffer_get_properties(buffer *b);

/*
 * Create and insert a new line into a buffer at a particular line number.
 * Subsequent lines are shifted accordingly.
 *
 * Tested by unit_buffer.insert_start, insert_middle, insert_end
 *
 * @param b  A pointer to buffer to insert into.
 * @param ln The linenumber of the new line.
 *
 * @return   0 on success, -1 on error.
 *
 */
int buffer_insert(buffer *b, lineno ln);

/*
 * Delete a specific line from a buffer. Subsequent lines are shifted
 * accordingly.
 *
 * @param b  A pointer to the buffer to delete a line from.
 * @param ln The linenumber of the line to delete.
 *
 * @return   0 on success, -1 on error.
 *
 */
int buffer_delete(buffer *b, lineno ln);

/*
 * Get a vector containing the contents of a particular line. The vector will
 * not be linked to the line in any way, and must be freed by the caller of this
 * function. The vector will be initialized with width sizeof(char), and should
 * be read as chars.
 *
 * Tested by unit_buffer.insert_start, insert_end
 *
 * @param b  A pointer to the buffer to return the contents of a line from.
 * @param ln The linenumber of the line to get.
 *
 * @return   A pointer to a vector containing the contents of the line. NULL on
 *           error.
 *
 */
vec *buffer_get_line(buffer *b, lineno ln);

/*
 * Set the contents of a line to contents provided in a vector. The vector must
 * be of width sizeof(char)
 *
 * Note that it is entirely possible to pass '\n' and other characters to
 * buffers with this method that will break things like UI and saving to files.
 * The buffer does not care about this, as it is a container of arbitrarydata.
 * Measures to prevent this from happening should be added via a hook or similar
 * if necessary.
 *
 * @param b  A pointer to the buffer to set the contents of a line in.
 * @param ln The linenumber of the line to set contents of. This line must have
 *           already been created with buffer_insert or equivilent.
 *
 * @return   0 on success, -1 on error.
 *
 */
int buffer_set_line(buffer *b, lineno ln, vec *l);

/*
 * Get the number of lines in a buffer.
 *
 * @param b A pointer to the buffer to count lines in.
 *
 * @return  The number of lines in the buffer. If there is an error, 0.
 *
 */
lineno buffer_len(buffer *b);

/*
 * Get the length of a specific line in a buffer, without having to call
 * buffer_get_line and create a vector for the contents of the line. This
 * is significantly faster, as the contents of the line do not have to be
 * copied.
 *
 * @param b  The buffer to find the length of a line in.
 * @param ln The line number of the line to find the length of.
 *
 * @return   The number of characters in the line. If there is an error, 0.
 *
 */
size_t buffer_len_line(buffer *b, lineno ln);

#endif /* BUFFER_BUFFER_H */
