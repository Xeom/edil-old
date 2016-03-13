#ifndef HOOK_H
# define HOOK_H

typedef struct hook_s hook;

typedef void (*callback_f)(vec *, hook *);

hook *hook_init(size_t numargs);

int hook_mount(hook *h, callback_f f);

int hook_call(hook *h, ...);

#endif /* HOOK_H */
