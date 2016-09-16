#if !defined(BUFFER_CHUNK_H)
# define BUFFER_CHUNK_H
# include "head.h"

# include "buffer/line.h"
# include "container/vec.h"

/* A chunk is a linked list container for vectors of lines.                   *
 *                                                                            *
 * Chunks automatically distribute lines between their vectors to keep        *
 * insertion time for any particular chunk into the vector fairly constant.   *
 * They will create new chunks and add them to their chain if necessary.      *
 * They will also delete chunks if there are not enough lines to fill all of  *
 * them to a minimum usage.                                                   *
 *                                                                            *
 * As a result, insertion of a new line is constant-time, since chunks are    *
 * kept below a constant size, the lookup of a line from linenumber is O(n),  *
 * though n will be small as it is divided by the length of each chunk, and   *
 * since buffers store the last visited chunk, lookup is O(1) if the lines    *
 * are in the same chunk (generally true of all the lines on the screen,      *
 * which all need to be fetched at once to display the buffer.                *
 *                                                                            *
 * Chunks are a subtype of vector, so vector operations can be used to insert *
 * or removed from them using vector functions. The length of a chunk can be  *
 * found using vec_len(chunk) for example.                                    */

/*
 * Initialize and return a new chunk.
 *
 * @return A pointer to the new chunk.
 *
 */
chunk *buffer_chunk_init(void);

/*
 * Free a chunk and all other chunks connected to it in its linked list.
 *
 * @param c A pointer to the chunk to begin the massacre at.
 *
 */
void buffer_chunk_free(chunk *c);

/*
 * Insert a line into a chunk at a specific offset. The offset must be between 0
 * and the length of the chunk.
 *
 * This function should not be used, as it does not call the
 * buffer_line_on_insert hooks, which various higher level systems may rely on.
 * buffer_insert should be used instead (it calls this function.)
 *
 * @param c      A pointer to the chunk to insert a line into (the exact chunk,
 *               not just a chunk in the same chain)
 * @param offset The offset to insert into the chunk at. 0 is the start of the
 *               chunk, and vec_len(c) will append to the end ofthe chunk.
 *
 * @return       0 on success, -1 on failure.
 *
 */
int buffer_chunk_insert_line(chunk *c, lineno offset);

/*
 * Delete a line from a chunk. The offset must be between 0 and one less than
 * the length of the chunk.
 *
 * This function should not be used, as it does not call the
 * buffer_line_on_delete hooks, which various higher level systems may
 * rely on. buffer_delete should be used instead (it calls this function.)
 *
 * @param c      A pointer to the chunk to delete a line from. (the exact chunk,
 *               not just a chunk in the same chain)
 * @param offset The offset into the chunk of the line to delete. 0 will delete
 *               the first line of the chunk, and vec_len(c) - 1 will delete the
 *               last line.
 *
 * @return       A pointer to a valid chunk. c may be invalid after being used
 *               this function, so should be replaced with the return value.
 *               NULL on error. If this function returns NULL, c is still a
 *               pointer. (though it may be messed up in a way that caused the
 *               error)
 *
 */
chunk *buffer_chunk_delete_line(chunk *c, lineno offset);

/*
 * Get a vec(char) containing the contents of a line at a particular offset in
 * a chunk. The offset must be between 0 and one less than the length of the
 * chunk.
 *
 * See the documentation for buffer_line_get_vec for further details -
 * this function calls it on the appropriate line.
 *
 * @param c      A pointer to the chunk to get a line from. (the exact chunk,
 *               not just a chunk in the same chain)
 * @param offset The offset into the chunk of the line to get the contents of.
 *               0 will return the contents of the first line, vec_len(c) - 1
 *               will return the last line's contents.
 *
 * @return       A pointer to a vector of chars containing the contents of the
 *               line. NULL on error.
 *
 */
vec *buffer_chunk_get_line(chunk *c, lineno offset);

vec *buffer_chunk_get_line_chars(chunk *c, lineno offset);

/*
 * Copy the contents of a vec(char) into a line at a particular offset in this
 * chunk. The offset must be between 0 and one less than the length of the
 * chunk. The line must exist to have its contents set.
 *
 * See the documentation for buffer_line_set_vec for further details - this
 * function calls it on the appropriate line.
 *
 * This function should not be used, as it does not call the
 * buffer_line_on_change hooks, which various higher level systems may rely on.
 * buffer_set_line should be ued instead (it calls this function.)
 *
 * @param c      A pointer to the chunk to set the contents of a line in (the
 *               exact chunk, not just a chunk in the same chain)
 * @param offset The offset into the chunk of the line to set the contents of.
 *               0 will set the contents of the first line, vec_len(c) - 1 will
 *               will set the contents of the last line.
 * @param v      A vector, of width sizeof(char), containing the new contents of
 *               the line.
 *
 * @return       0 on success, -1 on error.
 *
 */
int buffer_chunk_set_line(chunk *c, lineno offset, vec *v);

int buffer_chunk_set_line_chars(chunk *c, lineno offset, vec *v);

/*
 * Get a chunk that contains a particular linenumber from a chunk in the same
 * chain as it. If the linenumber is greater than the amount of lines in the
 * chain of chunks, the last chunk is returned.
 *
 * @param c  A pointer to a chunk in the chain to search for a chunk containing
 *           a particular line.
 * @param ln The linenumber to search for a chunk containing.
 *
 * @return   A pointer to the chunk containing the linenumber searched for. NULL
 *           on error.
 */
chunk *buffer_chunk_get_containing(chunk *c, lineno ln);

/*
 * Converts a linenumber to an offset for a particular chunk. The result is not
 * guaranteed to exist. If a linenumber is not in a chunk, the result of this
 * function is undefined.
 *
 * @param c  A pointer to the chunk to convert into an offset relative to.
 * @param ln The linenumber to convert into a relative offset.
 *
 * @return   The offset relative to c of ln. 0 on error.
 *
 */
lineno buffer_chunk_lineno_to_offset(chunk *c, lineno ln);

/*
 * Converts an offset relative to a chunk into a linenumber for a chain of
 * chunks. The linenumber is not guarenteed to exist if the offset is greater
 * than the length of the chunk.
 *
 * @param c      A pointer to the chunk that the offset is relative to.
 * @param offset An offset relative to the start of the chunk.
 *
 * @return       The absolute linenumber of offset, where 0 is the start of
 *               the chain of chunks c is in.
 *
 */
lineno buffer_chunk_offset_to_lineno(chunk *c, lineno offset);

/*
 * Get the total amount of lines in a chain of chunks.
 *
 * @param c A pointer to a chunk in the chain of chunks to count lines of.
 *
 * @return  The total number of lines in all chunks in the chain. 0 on error.
 *
 */
lineno buffer_chunk_get_total_len(chunk *c);

#endif /* BUFFER_CHUNK_H */
