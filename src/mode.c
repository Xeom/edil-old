#include <stdlib.h>

#include "err.h"
#include "io/keymap.h"

#include "mode.h"

struct mode_s
{
    char   *name;
    int     pri;
    keymap *map;
    hook    on_activate;
    hook    on_deactivate;
};

vec   *mode_active;

/* * THINGS TODO * *
 * Sorted Vec      *
 * This mode shit  *
 * * * * * * * * * */

mode *mode_init(int priority, char *name)
{
    mode *new;

    hook_add(h, 1);

    new = malloc(sizeof(mode));

    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);

    new->map = keymap_init();

    new->on_activate   = h;
    new->on_deactivate = h;

    return new;
}

int mode_activate(mode *m)
{
    size_t index;

    if (!mode_active)
        TRACE_PTR(mode_active = vec_init(sizeof(mode *)),
                  return -1);

    ASSERT(!vec_contains(mode_active, m), high, return -1);
    index = vec_len(mode_active);

    /* CBA to bisort episode 2 */
    while (index--)
    {
        mode **curr;

        curr = vec_item(mode_active, index);

        if ((*curr)->pri <= m->pri)
            break;
    }

    vec_insert(mode_active, index + 1, 1, &m);

    hook_call(m->on_activate, m);

    return 0;
}

int mode_deactivate(mode *m)
{
    size_t index;

    index = vec_len(mode_active);

    while (index--)
    {
        mode **curr;

        curr = vec_item(mode_active, index);

        if (*curr == m)
        {
            vec_delete(mode_active, index, 1);
            hook_call(m->on_deactivate, &m);

            return 0;
        }
    }

    return -1;
}

mode *mode_handle_press_last;

int mode_handle_press(key k)
{
    int res;

    if (mode_handle_press_last)
    {
        res = keymap_press(mode_handle_press_last->map, k);

        switch (res)
        {
        case 2: mode_handle_press_last = NULL;
        case 1: return 0;
        }
    }

    vec_foreach(mode_active, mode *, m,
                if (m == mode_handle_press_last)
                    continue;

                TRACE_INT(res = keymap_press(m->map, k),
                          return -1);

                if (res == 1)
                    mode_handle_press_last = m;

                if (res == 2)
                    mode_handle_press_last = NULL;

                if (res)
                    return 0;
        );

    return 0;
}

int mode_free(mode *m)
{
    free(m->name);
    m->name = NULL;

    hook_free(m->on_activate);
    hook_free(m->on_deactivate);

    keymap_free(m->map);

    if (mode_handle_press_last == m)
        mode_handle_press_last = NULL;

    return 0;
}
