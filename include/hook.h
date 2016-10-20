#if !defined(HOOK_H)
# define HOOK_H
# include "head.h"

# include "container/vec.h"

# define HOOK_PRIORITY_MAX 1000

typedef unsigned short priority;
typedef struct hook_s hook;

/* A function pointer to be called by hooks */
typedef void (*hook_f)(vec *, hook);

/* Hooks are a key part of edil's flexibility and extensibility.              *
 *                                                                            *
 * A hook can have multiple function pointers mounted to it, each with a      *
 * priority. When a hook is called, with a set of parameters, each function   *
 * is called with these parameters.                                           *
 *                                                                            *
 * This allows for components to be dynamically inserted at many different    *
 * points in edil.                                                            *
 *                                                                            *
 * Generally, hooks are defined in the form 'extern hook h;'. Python          *
 * manipulates these C hooks as pointers to the shared object.                *
 *                                                                            *
 * All arguments are passed to hooks as pointers.                             */

/* A structure to represent a hook. */
struct hook_s
{
    vec   *functs;  /* A vector containing details of the *
                     * functions mounted to this hook.    */

    size_t numargs; /* The number of arguments accepted by this hook. */
};

/* Used to create a new blank hook that accepts n arguments. */
#define hook_add(name, nargs) hook name = {NULL, nargs}

/*
 * Free all hooks and shut down the hook system.
 *
 * @return 0 on success, -1 on failure.
 *
 */
int hook_killsys(void);

/*
 * Free a hook. Note that this takes a hook, and not a hook * as an argument.
 * This function doesn't actually free the pointer to the hook structure, but
 * rather frees the pointers in the hook system relating to the hook.
 *
 * @param h The hook to free.
 *
 */
void hook_free(hook h);

/*
 * Mount a function onto a hook with a specific priority.
 *
 * @param h   A pointer to the hook to mount the function onto.
 * @param f   A function pointer to the function to mount. The function should
 *            accept a vector of pointer arguments, and the hook h.
 * @param pri The priority for the function mounted on the hook. Should be
 *            between 0 and 1000. Functions mounted with lower priorities are
 *            called first (100 before 900).
 *
 * @return    0 on success, -1 on failure.
 *
 */
int hook_mount(hook *h, hook_f f, priority pri);

/*
 * Unmount a function from a hook.
 *
 * @param h A pointer to the hook to unmount a function from.
 * @param f A pointer to the function to unmount.
 *
 */
int hook_unmount(hook *h, hook_f f);

/*
 * Call a hook with a set of parameters.
 *
 * @param h   The hook to call.
 * @param ... A number of pointer parameters to call the hook with. There must
 *            be the correct number for that specific hook.
 *
 * @return    0 on success, -1 on failure.
 *
 */
int hook_call(hook h, ...);

#endif /* HOOK_H */
