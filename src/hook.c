#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "container/vec.h"

#include "hook.h"

typedef struct hook_fcont_s hook_fcont;

struct hook_fcont_s
{
    hook_f funct;
    priority pri;
};

struct hook_s
{
    vec   *functs;
    size_t numargs;
};

void hook_call_funct(hook *h, hook_fcont f, vec *args);

hook *hook_init(size_t numargs)
{
    hook *rtn;

    rtn = malloc(sizeof(hook));

    rtn->functs   = vec_init(sizeof(hook_fcont));
    rtn->numargs  = numargs;

    return rtn;
}

void hook_free(hook *h)
{
    vec_free(h->functs);
    free(h);
}

int hook_mount(hook *h, hook_f f, priority pri)
{
    size_t     index;
    vec       *functs;
    hook_fcont cont;

    cont.funct = f;
    cont.pri   = pri;

    functs = h->functs;
    index  = vec_len(functs);

    /* In today's episode of "I really can't be fucked to bi-search..." */
    while (index--)
    {
        hook_fcont *curr;

        curr = vec_item(functs, index);

        if (curr->pri <= pri)
            break;
    }

    ++index;

    vec_insert(functs, index, 1, &cont);

    return 0;
}

int hook_unmount(hook *h, hook_f f)
{
    vec_foreach(h->functs, hook_fcont, cont,
                if (cont.funct == f)
                {
                    vec_delete(h->functs, _vec_index, 1);
                    return 0;
                }
        );

    return 0;
}

int hook_call(hook *h, ...)
{
    va_list args;
    vec    *argvec;
    size_t  numargs;

    va_start(args, h);

    numargs = h->numargs;
    argvec  = vec_init(sizeof(void *));

    while (numargs--)
        vec_insert_end(argvec, 1, va_arg(args, void *));

    vec_foreach(h->functs, hook_fcont, cont,
                hook_call_funct(h, cont, argvec));

    va_end(args);
    vec_free(argvec);

    return 0;
}

void hook_call_funct(hook *h, hook_fcont f, vec *args)
{
    f.funct(args, h);
}
