#if !defined(BUFFER_CHUNK_H)
# define BUFFER_CHUNK_H
# include "head.h"

# include "buffer/line.h"
# include "container/vec.h"

/*
 * A chunk is a container for a vector of lines.
 *
 * Chunks automatically distribute lines between their vectors to even out their usages.
 * Using chunks means that inserting a line is not an O(n), where n is the number of lines,
 * operation, but is O(1)-ish, since the fairly constant amount of lines per chunk is all
 * that needs to be moved.
 *
 * Chunks form a linked list, and each chunk keeps track of the linenumber of the first
 * line of that chunk. These linenumbers must be updated after each insertion, which kinda makes
 * insertion O(n) not O(1), but there are going to be like 20 chunks, so it's not much of an n.
 *
 */

/*
 * Initialize and return a new chunk.
 *
 */
chunk *buffer_chunk_init(void);

/*
 * Get the number of lines in a chunk.
 *
 * c is the chunk we want the length of.
 *
 */
size_t chunk_len(chunk *c);

/*
 * Free a chunk and all other chunks connected to it in its linked list.
 *
 * c is the chunk to massacre.
 *
 */
void buffer_chunk_free(chunk *c);

/*
 * Insert a line into a chunk at a specific offset. The offset must be between 0 and the length
 * of the chunk. This function should not be used, as it does not call the buffer_line_on_insert
 * hook, which various higher level systems may rely on. buffer_insert should be used instead
 * (it calls this function.)
 *
 * c is the chunk to insert a line into.
 *
 * offset is the offset (0-chunk_len(c)) to insert at.
 *
 */
int buffer_chunk_insert_line(chunk *c, lineno offset);

/*
 * Delete a line from a chunk. The offset must be between 0 and one less than the length of the
 * chunk. This function should not be used, as it does not call the buffer_line_on_delete hook,
 * which various higher level systems may rely on. buffer_delete should be used instead (it calls
 * this function.)
 *
 * c is the chunk to delete a line from.
 *
 * offset is the offset of the line to delete.
 *
 */
chunk *buffer_chunk_delete_line(chunk *c, lineno offset);

/*
 * Get a vec(char) containing the contents of a line at a particular offset in this chunk.
 *
 * c is the chunk to find a line in.
 *
 * offset is the offset (from 0 to chunk_len(c) - 1) of the line.
 *
 */
vec *buffer_chunk_get_line(chunk *c, lineno offset);

/*
 * Copy the contents of a vec(char) into a line at a particular offset in this chunk.
 *
 * c is the chunk to set a line in.
 *
 * offset is the offset (from 0 to chunk_len(c) - 1) of the line.
 *
 * v is the vec(char) to copy the contents of the line from.
 *
 */
int buffer_chunk_set_line(chunk *c, lineno offset, vec *v);

/*
 * Get a chunk that contains a particular linenumber from a chunk in the same chain
 * as it.
 *
 * c is a chunk in the same chain as the chunk we want.
 *
 * ln is the linenumber that we want to find.
 *
 */
chunk *buffer_chunk_get_containing(chunk *c, lineno ln);

/*
 * Converts a linenumber to an offset in a particular chunk. The result is not guaranteed to
 * exist.
 *
 * c is the chunk to convert into an offset for.
 *
 * ln is the linenumber to convert.
 *
 */
lineno buffer_chunk_lineno_to_offset(chunk *c, lineno ln);

/*
 * Converts an offset to a linenumber in a particular chunk. The result is not guaranteed to 
 * exist.
 *
 * c is the chunkk to convert from an offset for.
 *
 * offset is the offset to convert.
 *
 */
lineno buffer_chunk_offset_to_lineno(chunk *c, lineno offset);

/*
 * Get the total amount of lines in a chain of chunks.
 *
 * c is a chunk in the chain of chunks we want to get the length of.
 *
 */
lineno buffer_chunk_get_total_len(chunk *c);

#endif /* BUFFER_CHUNK_H */
