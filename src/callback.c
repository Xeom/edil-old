#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "container/vec.h"
#include "hook.h"

#include "callback.h"

void *callback_call_funct(callback cb, callback_f f, vec *args);

int callback_mount(callback *cb, callback_f f)
{
    cb->funct = f;

    return 0;
}

int callback_unmount(callback *cb)
{
    cb->funct = NULL;

    return 0;
}

void *callback_call(callback cb, ...)
{
    void   *rtn;
    va_list args;
    vec    *argvec;
    size_t  numargs;

    va_start(args, cb);

    numargs = cb.numargs;
    argvec  = vec_init(sizeof(void *));

    while (numargs--)
    {
        void *arg;
        arg = va_arg(args, void *);
        vec_insert_end(argvec, 1, &arg);
    }

    rtn = callback_call_funct(cb, cb.funct, argvec);

    va_end(args);
    vec_free(argvec);

    return rtn;
}

void *callback_call_funct(callback cb, callback_f f, vec *args)
{
    if (f == NULL)
        return NULL;

    return f(args, cb);
}
