#include <stdarg.h>
#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "hook.h"

typedef struct hook_fcont_s hook_fcont;

struct hook_fcont_s
{
    hook_f funct;
    priority pri;
};

static vec *hook_vecs_to_free = NULL;

static void hook_call_funct(hook h, hook_fcont f, vec *args);

int hook_initsys(void)
{
    hook_vecs_to_free = vec_init(sizeof(vec *));

    return 0;
}

int hook_killsys(void)
{
    vec_foreach(hook_vecs_to_free, vec *, v,
                vec_free(v));

    vec_free(hook_vecs_to_free);

    return 0;
}

void hook_free(hook h)
{
    vec_free(h.functs);
}

int hook_mount(hook *h, hook_f f, priority pri)
{
    size_t     index;
    vec       *functs;
    hook_fcont cont;

    ASSERT_PTR(h, high, return -1);
    cont.funct = f;
    cont.pri   = pri;

    functs = h->functs;

    if (functs == NULL)
    {
        functs = h->functs = vec_init(sizeof(hook_fcont));
        vec_insert_end(hook_vecs_to_free, 1, &functs);
    }

    index  = vec_len(functs);

    /* In today's episode of "I really can't be fucked to bi-search..." */
    while (index--)
    {
        hook_fcont *curr;

        curr = vec_item(functs, index);

        if (curr->pri <= pri)
            break;
    }

    vec_insert(functs, index + 1, 1, &cont);

    return 0;
}

int hook_unmount(hook *h, hook_f f)
{
    vec_foreach(h->functs, hook_fcont, cont,
                if (cont.funct != f)
                    continue;

                vec_delete(h->functs, _vec_index, 1);

                return 0;
        );

    return 0;
}

int hook_call(hook h, ...)
{
    va_list args;
    vec    *argvec, *completed;
    size_t  numargs;

    va_start(args, h);

    if (h.functs == NULL)
        return 0;

    numargs = h.numargs;
    argvec  = vec_init(sizeof(void *));

    while (numargs--)
    {
        void *arg;
        arg = va_arg(args, void *);
        vec_insert_end(argvec, 1, &arg);
    }

    completed = vec_init(sizeof(hook_f));

    /* Terrible O(n^2)-ness */
start_search: /* hahahaahahahhahaahaohgod */
    /* Iterate across all functions */
    vec_foreach(h.functs, hook_fcont, cont,
                /* See if we've already executed the function */
                if (vec_contains(completed, &(cont.funct)))
                    continue;

                /* If not, we add it to our list of completed functions,
                 * and execute it */
                vec_insert_end(completed, 1, &(cont.funct));
                hook_call_funct(h, cont, argvec);

                /* We then go back to the start. Goto in order to break *
                 * out of two loops                                     */
                goto start_search; /* AAAAAA */
        )

    va_end(args);

    vec_free(argvec);
    vec_free(completed);

    return 0;
}

void hook_call_funct(hook h, hook_fcont f, vec *args)
{
    f.funct(args, h);
}
