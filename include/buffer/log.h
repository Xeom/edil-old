#if !defined(BUFFER_LOG_H)
# define BUFFER_LOG_H
# include "head.h"

# include "cursor/cursor.h"
# include <stdio.h>

/* The buffer_log system provides an easy way to dump data from pipes into    *
 * buffers. The system simply exposes methods to create pipes that have       *
 * listeners hooked to them, and dump anything written to them into the       *
 * buffer. Note that newlines will be interpreted as newlines and not         *
 * escapes.                                                                   *
 *                                                                            *
 * It is also worth noting that the pipes will only be read on the next poll, *
 * not instantly, allowing for other events to interrupt and coexist with     *
 * logging.                                                                   */

/*
 * Initialize the buffer log system.
 *
 * @return 0 on success, -1 on error.
 *
 */
int buffer_log_initsys(void);

/*
 * Create and return a stream that dumps its contents into a cursor. The stream
 * is opened for writing with no buffer.
 *
 * @param cur A pointer to the cursor to dump the contents of the pipe into.
 *
 * @return    The stream that will dump its contents into cur.
 *
 */
FILE *buffer_log_point_stream(cursor *cur);

#endif /* BUFFER_LOG_H */
