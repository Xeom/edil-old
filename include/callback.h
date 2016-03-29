#ifndef CALLBACK_H
# define CALLBACK_H

typedef struct callback_s callback;
typedef void *(*callback_f)(vec *, callback);

struct callback_s
{
    callback_f funct;
    size_t     numargs;
};

#define callback_add(name, nargs) callback name = {NULL, nargs}

int callback_mount(callback *cb, callback_f f);

int callback_unmount(callback *cb);

void *callback_call(callback cb, ...);

#endif /* CALLBACK_H */
