#include <stdlib.h>

#include "err.h"
#include "io/keymap.h"

#include "mode.h"

typedef struct mode_mountf_s mode_mountf;

struct mode_mountf_s
{
    hook_f funct;
    hook *h;
};

struct mode_s
{
    uint    active:1;
    char   *name;
    priority pri;
    keymap *map;
    hook    on_activate;
    hook    on_deactivate;
    vec     mounts;
};

vec   *mode_active;

hook_add(mode_on_free, 1);
hook_add(mode_on_init, 1);

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
    vec_create(&(new->mounts), sizeof(mode_mountf));

    new->on_activate   = h;
    new->on_deactivate = h;

    hook_call(mode_on_init, new);

    return new;
}

int mode_activate(mode *m)
{
    size_t index;
    static const mode cmpmap = {.pri = 0x3ff};

    if (!mode_active)
        TRACE_PTR(mode_active = vec_init(sizeof(mode *)),
                  return -1);

    ASSERT(!vec_contains(mode_active, m), high, return -1);
    index = vec_len(mode_active);

    index = vec_bisearch(mode_active, m, &cmpmap);
    vec_insert(mode_active, index, 1, &m);

    vec_foreach(&(m->mounts), mode_mountf, mount,
                hook_mount(mount.h, mount.funct, m->pri));

    m->active = 1;
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

            vec_foreach(&(m->mounts), mode_mountf, mount,
                hook_unmount(mount.h, mount.funct));

            m->active = 0;

            hook_call(m->on_deactivate, &m);

            return 0;
        }
    }

    return -1;
}


hook *mode_on_activate(mode *m)
{
    return &(m->on_activate);
}

hook *mode_on_deactivate(mode *m)
{
    return &(m->on_deactivate);
}

static mode *mode_handle_press_last;

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
    if (m->active)
        mode_deactivate(m);

    hook_call(mode_on_free, m);

    free(m->name);
    m->name = NULL;

    hook_free(m->on_activate);
    hook_free(m->on_deactivate);

    keymap_free(m->map);

    if (mode_handle_press_last == m)
        mode_handle_press_last = NULL;

    return 0;
}

int mode_add_mount(mode *m, hook *h, hook_f f)
{
    mode_mountf *new;

    vec_insert_end(&(m->mounts), 1, NULL);
    new = vec_item(&(m->mounts), vec_len(&(m->mounts)) - 1);

    new->funct = f;
    new->h     = h;

    return 0;
}

int mode_remove_mount(mode *m, hook *h, hook_f f)
{
    size_t ind;

    ind = vec_len(&(m->mounts));

    while (ind--)
    {
        mode_mountf *mount;

        mount = vec_item(&(m->mounts), ind);

        if (mount->funct == f &&
            mount->h     == h)
        {
            if (m->active)
                hook_unmount(h, f);

            vec_delete(&(m->mounts), ind, 1);
        }
    }

    return 0;
}
