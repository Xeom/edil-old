#if !defined(BUFFER_LINE_H)
# define BUFFER_LINE_H
# include "head.h"

# include <stddef.h>
# include "container/vec.h"

typedef size_t lineno;
typedef size_t colno;

typedef struct line_s line;
typedef struct chunk_s chunk;

/* Lines are used by buffers to store their text data.                        *
 *                                                                            *
 * Each line contains one line of text, and so is assumed to end in a newline *
 * character of some sort. The addition of this character should be handled   *
 * by file serialization. The character is not included in lines.             *
 *                                                                            *
 * Lines are however agnostic to \n and other special characters, and some,   *
 * such as \f could be added to the end of a line to signify that the line    *
 * break is to be treated specially. Other systems however, such as ui etc.   *
 * will need to handle this, not lines.                                       *
 *                                                                            *
 * Lines are dumb containers, and provide no checking for misplaced newlines, *
 * special characters, or anything being on fire. Keep in mind that it is     *
 * entirely possible that they could contain any bytes whatsoever.            */

/*
 * Initialize and return a new, empty buffer line.
 *
 * @return A pointer to the new line.
 *
 */
line *buffer_line_init(void);

/*
 * Free a buffer line's memory. No error is thrown if the function is passed
 * NULL, however a line that is already free'd may cause an issues, depending on
 * how upset the OS likes to be about that. (Yea don't do that okay?)
 *
 * @param l A pointer to the line to free.
 *
 */
void buffer_line_free(line *l);

/* Rather than providing an interface to get an iterator to a string to edit  *
 * contents of a line, we provide a way to get a vector of chars, containing  *
 * the contents of the line. This can then we modified in any way without     *
 * affecting the line. Another function then accepts a vector and sets the    *
 * contents of a line to the contents of the vector.                          *
 *                                                                            *
 * This way, it is far easier and more reliable to add hooks that are called  *
 * when a line's content is modified, and the hook will be called far less    *
 * often spuriously.                                                          */

/*
 * Get a vector containing the contents of a particular line. Note that the
 * calling function must free this vector.
 *
 * @param l A pointer to the line to get the contents of.
 *
 * @return  A poiner to a vector of width sizeof(char), containing the contents
 *          of the line.
 *
 */
vec *buffer_line_get_vec(line *l);

vec *buffer_line_get_vec_chars(line *l);

/*
 * Set the contents of a line to the contents of a vector
 *
 * This function should not be used directly on a line in a buffer, as it does 
 * not call the buffer_line_on_change hook. buffer_set_line() should be used for
 * this instead.
 *
 * @param l A pointer to the line to set the contents of.
 * @param v A pointer to a vec of chars containing the new contents of the line.
 *
 * @return  0 on success, -1 on error.
 *
 */
int buffer_line_set_vec(line *l, vec *v);

/*
 * Set the contents of a line to the utf8-encoded contents of an int32_t vector
 *
 * This function should not be used directly on a line in a buffer, as it does
 * not call the buffer_line_on_change hook. buffer_set_line() should be used for
 * this instead.
 *
 * @param l A pointer to the line to set the contents of.
 * @param v A pointer to a vec of int32_ts containing the new contents of the 
 *          line.
 *
d
 *
 */
int buffer_line_set_vec_chars(line *l, vec *v);

/*
 */
size_t buffer_line_len(line *l);

#endif /* BUFFER_LINE_H */
