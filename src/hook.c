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
    vec_push(h->functs, &f);

    return 0;
}

int hook_call(hook *h, ...)
{
    va_list args;
    vec    *argvec;
    size_t  numargs, numfuncts;

    va_start(args, h);

    numargs = h->numargs;
    argvec  = vec_init(sizeof(void *));

    while (numargs--)
        vec_push(argvec, va_arg(args, void *));

    numfuncts = vec_len(h->functs);

    while (numfuncts--)
        hook_call_funct(h, *(callback_f*)vec_get(h->functs, numfuncts), argvec);

    va_end(args);

    return 0;
}

void hook_call_funct(hook *h, callback_f f, vec *args)
{
    f(args, h);
}
