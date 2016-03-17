#ifndef WINCONT_H
# define WINCONT_H
#include "line.h"
#include "textcont.h"

typedef struct wincont_s wincont;

/* Initialize the window container system */
int wincont_initsys(void);

/* Kill the window container system */
int wincont_killsys(void);

/*
 * Initializes and returns a new window content container.
 * The new wincont is added to the ring of winconts, or if wincont_root is NULL,
 * it becomes the first in the ring.
 *
 * text is a pointer to the textcont the window content container should contain
 *
 */
wincont *wincont_init(textcont *text);

/*
 * Copies a wincont, adding it to the ring, allowing for the "forking" of a window, so
 * two copies of the same window may be viewed at different positions at the same time.
 *
 * cont is the wincont to clone
 *
 */
wincont *wincont_clone(wincont *cont);

/*
 * Returns the next wincont in the ring.
 *
 * cont is the wincont whose next sister we want to get.
 *
 */
wincont *wincont_next(wincont *cont);


/*
 * Returns the previous wincont in the ring.
 *
 * cont is the wincont whose previous sister we want to get.
 *
 */
wincont *wincont_prev(wincont *cont);

/*
 * Get the root wincont. This function is useful when initializing the
 * window system, so you can get the root wincont without initializing a second one.
 *
 */
wincont *wincont_get_root(void);

/*
 * Gets a particular line of a wincont.
 *
 * cont is the wincont to get a line from.
 *
 * ln is the line number we seek. 0 would be the first line of the wincont, and we
 * increase downwards from there.
 *
 */
line *wincont_get_line(wincont *cont, lineno ln);

/*
 */
const char *wincont_get_line_text_const(wincont *cont, line *l);

/*
 */
textcont *wincont_get_textcont(wincont *cont);

#endif /* WINCONT_H */
