#if !defined(MODE_H)
# define MODE_H
# include "head.h"

# include "io/keymap.h"
# include "io/key.h"

typedef struct mode_s mode;

/* Called before a mode is free'd.                          *
 * One argument - A pointer to the mode about to be free'd. */
extern hook mode_on_free;

/* Called after a new mode is initialized.   *
 * One argument - A pointer to the new mode. */
extern hook mode_on_init;

/*
 * Initialize a new mode.
 *
 * @param priority The priority of the mode, from 0 to 1000. Modes with lower
 *                 priority values have their keymaps tried for key-comboss
 *                 first.
 * @param name     A pointer to a NULL terminated string containing the name of
 *                 the mode.
 *
 * @return         A pointer to the new mode, or NULL on error.
 *
 */
mode *mode_init(int priority, char *name);

/*
 * Activates a mode.
 *
 * When activated, a mode's on_activate hook is called, its hook->function
 * mounts are mounted, and its keymap activates for use with mode_handle_press.
 * Each mode should only be activated once.
 *
 * @param m A pointer to the mode to activate.
 *
 * @return  0 on success, -1 on failure.
 *
 */
int mode_activate(mode *m);

/*
 * Deactivates a mode. Effectively the opposite of mode_activate.
 *
 * When deactivate, a mode's on_deactivate hook is called, its hook->function
 * mounts are unmounted, and its keymap deactivates so that it is no longer
 * used by mode_handle_press.
 *
 * @param m A pointer to the mode to deactivate.
 *
 * @return  0 on success, -1 on failure.
 *
 */
int mode_deactivate(mode *m);

/*
 * Get a mode's on_activate hook.
 *
 * A mode's on_activate hook is called whenever the mode is activated. This
 * function gets a pointer to that hook for any specific mode.
 *
 * @param m A pointer to the mode to get a on_activate hook for.
 *
 * @return  A pointer to the hook.
 *
 */
hook *mode_get_on_activate(mode *m);

/*
 * Get a mode's on_deactivate hook.
 *
 * A mode's on_deactivate hook is called whenever the mode is deactivated. This
 * function gets a pointer to that hook for any specific mode.
 *
 * @param m A pointer to the mode to get a on_deactivate hook for.
 *
 * @return  A pointer to the hook.
 *
 */
hook *mode_get_on_deactivate(mode *m);

/*
 * Get a mode's keymap.
 *
 * A mode's keymap is used to store key-bindings which are used when that mode
 * is active. This function should be used to alter the keybindings of the
 * mode's keymap, rather than for handling keypresses for the mode.
 * mode_handle_press should be used for that.
 *
 * @param m A pointer to the mode to get a keymap for.
 *
 * @return  A pointer to the keymap, NULL on error.
 *
 */
keymap *mode_get_keymap(mode *m);

/*
 * Use the mode-system to handle a keypress.
 *
 * This function checks all activated modes for one which will accept this 
 * keypress, and handles it appropriately.
 *
 * @param k The key pressed.
 *
 * @return  0 on success, -1 on failure.
 *
 */
int mode_handle_press(key k);

/*
 * Free a mode.
 *
 * This function removes reference to a specific mode from the mode system, and
 * frees the mode's keymap, memory, and hooks.
 *
 * @param m A pointer to the mode to free.
 *
 * @return  0 on success, -1 on error.
 *
 */
int mode_free(mode *m);

/*
 * Add a function and hook pair to mount while a mode is active.
 *
 * While the specified mode is activated, the specified function will be mounted
 * to the specified hook.
 *
 * @param m A pointer to the mode to add mounting pair to.
 * @param h A pointer to the hook to mount.
 * @param f A function pointer to the function to mount.
 *
 * @return  0 on success, -1 on error.
 *
 */
int mode_add_mount(mode *m, hook *h, hook_f f);

/*
 * Remove a function and hook mounting pair created by mode_add_mount from a
 * mode.
 *
 * @param m A pointer to the mode to remove a mounting pair from.
 * @param h A pointer to the hook to remove a mounting pair for.
 * @param f A function pointer to the function to remove a mounting pair for.
 *
 * @return  0 on success, -1 on error.
 *
 */
int mode_remove_mount(mode *m, hook *h, hook_f f);

#endif /* MODE_H */
