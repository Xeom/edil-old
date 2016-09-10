#include <stdlib.h>

#include "err.h"
#include "io/keymap.h"

#include "mode.h"

typedef struct mode_mountf_s mode_mountf;

/* Represents a pair of hook and function. When the mode activates, the *
 * funct mounts the hook, and when the mode deactivates, it unmounts.   */
struct mode_mountf_s
{
    hook_f funct; /* The function to mount to the hook. */
    hook *h;      /* The hook to mount the function on. */
};

struct mode_s
{
    uint    active:1;      /* whether the mode is activated or not ..   */
    char   *name;          /* The name of the mode                      */
    priority pri;          /* The priority of the mode's keybinds       */
    keymap *map;           /* A pointer to the mode's keymap            */
    hook    on_activate;   /* hook to call when the mode is activated   */
    hook    on_deactivate; /* hook to call when the mode is deactivated */
    vec     mounts;        /* A vector of mode_mountfs to mount when    *
                            * the mode is activated.                    */
};

/* A vector of currently active modes, ordered by priority. */
static vec *mode_active = NULL;

/* Define hooks for when modes are free'd and initialized. */
hook_add(mode_on_free, 1);
hook_add(mode_on_init, 1);

mode *mode_init(int priority, char *name)
{
    mode *new;

    /* Create a new hook to mount to the mode. *
     * This macro must be at the start of the  *
     * function as it declares h as a variable */
    hook_add(h_activate, 1);
    hook_add(h_deactivate, 1);

    ASSERT_PTR(new = malloc(sizeof(mode)), terminal, return NULL);

    /* Allo space for the name of the mode, and copy it */
    ASSERT_PTR(new->name = malloc(strlen(name) + 1), terminal,
               free(new);
               return NULL);
    strcpy(new->name, name);

    /* Initialize a new keymap for this mode... */
    TRACE_PTR(new->map = keymap_init(),
              free(new->name);
              free(new);
              return NULL);

    /* Create a vec to contain mode_mountfs for this mode */
    TRACE_PTR(vec_create(&(new->mounts), sizeof(mode_mountf)),
              free(new->name);
              free(new);
              return NULL);

    /* Put our new hooks in the mode as well.. */
    new->on_activate   = h_activate;
    new->on_deactivate = h_deactivate;

    /* Deactivate the new mode */
    new->active = 0;

    hook_call(mode_on_init, new);

    return new;
}

int mode_activate(mode *m)
{
    size_t index;

    /* Define a bitmask for comparing the priorities of modes. */
    static const mode cmpmap = {.pri = 0x3ff};

    /* If there is no mode_active vector, initialize one. */
    if (!mode_active)
        TRACE_PTR(mode_active = vec_init(sizeof(mode *)),
                  return -1);

    /* Check that our mode is not already active.  */
    ASSERT(!vec_contains(mode_active, &m), high, return -1);
    ASSERT(!m->active,                     high, return -1);

    /* Find a suitable index to insert our mode at, by comparing *
     * the priorities of the modes using the cmpmap bitmask.     */
    TRACE_IND(index = vec_bisearch(mode_active, &m, &cmpmap), return -1);

    /* Insert our new active mode! */
    TRACE_INT(vec_insert(mode_active, index, 1, &m), return -1);

    /* Mount all the mode_mountfs of the mode. */
    vec_foreach(&(m->mounts), mode_mountf, mount,
                hook_mount(mount.h, mount.funct, m->pri));

    /* Tell the mode it's active. */
    m->active = 1;

    /* Call the mode's on_activate hook */
    hook_call(m->on_activate, m);

    return 0;
}

int mode_deactivate(mode *m)
{
    size_t index;

    /* Iterate over the active modes in reverse order.               *
     * Reverse since more static modes will be nearer the beginning. */
    index = vec_len(mode_active);

    while (index--)
    {
        mode **curr;

        TRACE_PTR(curr = vec_item(mode_active, index), return -1);

        if (*curr == m)
        {
            /* If this is the mode we want, delete it from the index. */
            TRACE_INT(vec_delete(mode_active, index, 1), return -1);

            /* Unmount all its mode_mountfs */
            vec_foreach(&(m->mounts), mode_mountf, mount,
                hook_unmount(mount.h, mount.funct));

            /* Tell the mode it's deactivated. */
            m->active = 0;

            /* Call the mode's on_deactivate hook. */
            hook_call(m->on_deactivate, &m);

            return 0;
        }
    }

    return -1;
}


hook *mode_get_on_activate(mode *m)
{
    /* Check we have a valid pointer .. */
    ASSERT_PTR(m, high, return NULL);

    return &(m->on_activate);
}

hook *mode_get_on_deactivate(mode *m)
{
    /* Check we have a valid pointer .. */
    ASSERT_PTR(m, high, return NULL);

    return &(m->on_deactivate);
}

keymap *mode_get_keymap(mode *m)
{
    /* Check we have a valid pointer .. */
    ASSERT_PTR(m, high, return NULL);

    return m->map;
}

/* The last keymap that accepted an incomplete key-combo in     *
 * mode_handle_press,                                           *
 *                                                              *
 * This is declared outside of mode_handle_press so that we can *
 * clear it to NULL if we free that mode.                       */
static mode *mode_handle_press_last = NULL;

int mode_handle_press(key k)
{
    int res;

    /* If there is currently an incompleted keystroke, we'll hand this *
     * key to the appropriate map right away.                          */
    if (mode_handle_press_last && mode_handle_press_last->active)
    {
        res = keymap_press(mode_handle_press_last->map, k);

        TRACE_INT(res, return -1);

        switch (res)
        {
            /* If the keypress is completed, we need to reset the  *
             * mode_handle_press_last variable so we don't try and *
             * hand this keymap future keys.                       */
        case 2: mode_handle_press_last = NULL;
            /* If the keypress is continuing, or completed, we *
             * leave the function here and return success.     */
        case 1: return 0;
        }
    }

    /* If there's no key-combo to continue, we need to find a map *
     * to start a new key-combo with. To do this, we iterate over *
     * the maps in order of priority until one accepts our key.   */
    vec_foreach(mode_active, mode *, m,
                /* Don't send a mode the same keypress twice. *
                 * If it's mode_handle_press_last, we already *
                 * tried it with this keypress.               */
                if (m == mode_handle_press_last)
                    continue;

                /* Try to hand the keypress to this map. */
                TRACE_INT(res = keymap_press(m->map, k),
                          return -1);

                /* If it accepts it, but this is not the whole keypress, *
                 * we set mode_handle_press_last, and return.            */
                if (res == 1)
                    mode_handle_press_last = m;

                /* If this single keypress is a full key-combo,      *
                 * mode_handle_press_last should be NULL as we can't *
                 * continue the keypress.                            */
                if (res == 2)
                    mode_handle_press_last = NULL;

                if (res)
                    return 0;
        );

    return 0;
}

int mode_free(mode *m)
{
    /* Deactivate the mode if it's still active */
    if (m->active)
        mode_deactivate(m);

    /* Before we destruct the mode, call the free hook */
    hook_call(mode_on_free, m);

    /* Free the memory containing the mode's name */
    free(m->name);
    m->name = NULL;

    /* Free the mode's hooks */
    hook_free(m->on_activate);
    hook_free(m->on_deactivate);

    /* Free the mode's key bindings */
    keymap_free(m->map);

    /* Check that mode_handle_press_last isn't invalid */
    if (mode_handle_press_last == m)
        mode_handle_press_last = NULL;

    return 0;
}

int mode_add_mount(mode *m, hook *h, hook_f f)
{
    mode_mountf *new;

    ASSERT_PTR(m, high, return -1);
    ASSERT_PTR(h, high, return -1);
    ASSERT_PTR(f, high, return -1);

    /* Extend the m->mounts vector */
    TRACE_INT(vec_insert_end(&(m->mounts), 1, NULL), return -1);

    /* Get a pointer to the newly extended vector's item */
    new = vec_item(&(m->mounts), vec_len(&(m->mounts)) - 1);

    TRACE_PTR(new, return -1);

    /* Fill up the new hook_mountf */
    new->funct = f;
    new->h     = h;

    return 0;
}

int mode_remove_mount(mode *m, hook *h, hook_f f)
{
    size_t ind;

    ASSERT_PTR(m, high, return -1);
    ASSERT_PTR(h, high, return -1);
    ASSERT_PTR(f, high, return -1);

    TRACE_IND(ind = vec_len(&(m->mounts)), return -1);

    /* Iterate across all the mode's mode_mountfs */
    while (ind--)
    {
        mode_mountf *mount;

        TRACE_PTR(mount = vec_item(&(m->mounts), ind), return -1);

        /* If we've found the correct mode_mountf, delete it. */
        if (mount->funct == f &&
            mount->h     == h)
        {
            if (m->active)
                hook_unmount(h, f);

            TRACE_INT(vec_delete(&(m->mounts), ind, 1), return -1);
        }
    }

    return 0;
}
