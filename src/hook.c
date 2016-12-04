#include <stdarg.h>
#include <stdlib.h>

#include "container/vec.h"
#include "err.h"

#include "hook.h"

typedef struct hook_fcont_s hook_fcont;

/* A structure representing a function mounted to a hook. */
struct hook_fcont_s
{
    hook_f funct;
    priority pri;
};

/* A vector of vectors allocated by the hook system that need to be free'd *
 * when the system to killed.                                              */
static vec *hook_vecs_to_free = NULL;

/*
 * Call a function mounted on a hook with a specific set of arguments.
 *
 * @param h    The hook the function is mounted to.
 * @param f    A function container containing the function to call.
 * @param args A vector of pointers to arguments to call the functions with.
 *
 */
static void hook_call_funct(hook h, hook_fcont f, vec *args);

void hook_killsys(void)
{
    if (hook_vecs_to_free)
    {
        /* Free all the vectors we allocated */
        vec_foreach(hook_vecs_to_free, vec *, v,
                    vec_free(v));

        /* Free the vector we stored them in */
    }

    hook_vecs_to_free = NULL;
}

void hook_free(hook h)
{
    if (h.functs)
    {
        vec_free(h.functs);

        if (vec_contains(hook_vecs_to_free, &h.functs))
            vec_delete(hook_vecs_to_free,
                       vec_find(hook_vecs_to_free, &h.functs), 1);
    }
}

int hook_mount(hook *h, hook_f f, priority pri)
{
    size_t     index;
    vec       *functs;
    hook_fcont cont;

    static const hook_fcont cmpmap = {.pri = 0x3ff};

    ASSERT_PTR(h, high, return -1);
    cont.funct = f;
    cont.pri   = pri;

    functs = h->functs;

    if (functs == NULL)
    {
        functs    = vec_init(sizeof(hook_fcont));
        h->functs = functs;

        if (hook_vecs_to_free == NULL)
        {
            hook_vecs_to_free = vec_init(sizeof(vec *));
            atexit(hook_killsys);
        }

        vec_insert_end(hook_vecs_to_free, 1, &functs);
    }

    index = vec_bisearch(functs, &cont, &cmpmap);
    vec_insert(functs, index, 1, &cont);

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
