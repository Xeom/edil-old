#if !defined(BUFFER_LINE_H)
# define BUFFER_LINE_H
# include "head.h"

# include <stddef.h>
# include "container/vec.h"

typedef size_t lineno;
typedef size_t colno;

typedef struct line_s line;
typedef struct chunk_s chunk;

/*
 * Lines are structures used by buffers to store text data.
 *
 * Each line is assumed to end in a newline character (\r\n or \n or equiv.), which will be
 * added in serialization to and from files. This character is not included in lines.
 *
 * If other characters, such as \f are used to seperate lines, then they can be added at
 * the end of a line. File serializers and other systems using lines should be aware of this.
 *
 * Lines are dumb containers, and provide no checking for misplaced newlines, special characters,
 * or anything being on fire. Keep in mind that it is entirely possible that they could contain
 * any bytes whatsoever.
 *
 */

/*
 * Initialize and return a new, empty buffer line.
 *
 */
line *buffer_line_init(void);

/*
 * Free a buffer line's memory
 *
 * l is the line to free.
 *
 */
void buffer_line_free(line *l);

/*
 * The contents of a line are edited by getting a vector containing the characters of a line,
 * editing the vector, and then setting the contents of the line to the contents of the vector.
 *
 * This provides easy editing, as the vector is very manipulable, and means that the set
 * function can be attached to a hook to redraw a line, move cursors in the line etc. When
 * a line is changed.
 *
 */

/*
 * Get a vector representing the contents of the line.
 *
 * l is the line whose content we're gonna copy.
 *
 */
vec *buffer_line_get_vec(line *l);

/*
 * Set the content of a line to the contents of a vector. This function should not be used directly on
 * a line in a buffer or chunk, as it does not call the buffer_line_on_change hook, which higher level
 * systems may rely on. buffer_set_line should be used for this instead.
 *
 * l is the line whose content we want to set.
 *
 * v is a vec(char) which contains the new contents of the line.
 *
 */
int buffer_line_set_vec(line *l, vec *v);

#endif /* BUFFER_LINE_H */
