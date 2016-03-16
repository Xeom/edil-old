#ifndef HOOK_H
# define HOOK_H

#define HOOK_PRIORITY_MAX 1000

typedef unsigned short priority;
typedef struct hook_s hook;

typedef void (*hook_f)(vec *, hook *);

hook *hook_init(size_t numargs);

int hook_mount(hook *h, hook_f f, priority pri);

int hook_unmount(hook *h, hook_f f);

int hook_call(hook *h, ...);

#endif /* HOOK_H */
