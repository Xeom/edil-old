#ifndef CALLBACK_H
# define CALLBACK_H

typedef struct callback_s callback;

typedef void *(*callback_f)(vec *, callback *);

callback *callback_init(size_t numargs);

int callback_mount(callback *cb, callback_f f);

int callback_unmount(callback *cb);

void *callback_call(callback *cb, ...);

#endif /* CALLBACK_H */
