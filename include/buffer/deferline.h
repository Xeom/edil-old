#if !defined(BUFFER_DEFERLINE_H)
# define BUFFER_DEFERLINE_H
# include "head.h"

# include <stddef.h>

# include "container/vec.h"
# include "hook.h"

/* A deferline is a structure that is intended to allow multiple hooks to     *
 * alter the content of a line before it is drawn. The issue that is solved   *
 * by deferlines, is that if multiple things want to insert a string at a     *
 * certain index of a line before it is drawn, and they both insert           *
 * characters at that index, subsequent attempts to insert characters at      *
 * later indices will not work.                                               *
 *                                                                            *
 * To solve this, the deferline stores a set of indices and strings to insert *
 * at those indices, and a series of indices to delete, then dumps them all   *
 * at once, and makes the changes to the line starting with the latest        *
 * indices.                                                                   */

typedef struct deferline_s deferline;

extern hook buffer_deferline_on_draw;

int buffer_deferline_initsys(void);

deferline *buffer_deferline_init(vec *v);

vec *buffer_deferline_get_vec(deferline *dl);

void buffer_deferline_free(deferline *dl);

int buffer_deferline_insert(deferline *dl, size_t index, const char *str);

int buffer_deferline_delete(deferline *dl, size_t index);

int buffer_deferline_dump(deferline *dl);

#endif /* BUFFER_DEFERLINE_H */
