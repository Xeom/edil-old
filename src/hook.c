#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "vec.h"
#include "hook.h"

void hook_call_funct(hook *h, callback_f f, vec *args);

struct hook_s
{
    vec *functs;
    size_t numargs;
};

hook *hook_init(size_t numargs)
{
    hook *rtn;

    rtn = malloc(sizeof(hook));

    rtn->functs = vec_init(sizeof(callback_f));
    rtn->numargs = numargs;

    return rtn;
}

int hook_mount(hook *h, callback_f f)
{
    vec_insert(h->functs, vec_len(h->functs), 1, &f);

    return 0;
}

int hook_unmount(hook *h, callback_f f)
{
    size_t index;

    index = vec_find(h->functs, &f);

    vec_delete(h->functs, index, 1);

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
        vec_insert(argvec, vec_len(argvec), 1, va_arg(args, void *));

    vec_foreach(h->functs, callback_f, funct,
                hook_call_funct(h, funct, argvec));

    va_end(args);

    return 0;
}


void hook_call_funct(hook *h, callback_f f, vec *args)
{
    f(args, h);
}
