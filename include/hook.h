#if !defined(HOOK_H)
# define HOOK_H

#include "container/vec.h"

#define HOOK_PRIORITY_MAX 1000

typedef unsigned short priority;
typedef struct hook_s hook;

typedef void (*hook_f)(vec *, hook);

struct hook_s
{
    vec   *functs;
    size_t numargs;
};

#define hook_add(name, nargs) hook name = {NULL, nargs}

int hook_initsys(void);

int hook_killsys(void);

void hook_free(hook h);

int hook_mount(hook *h, hook_f f, priority pri);

int hook_unmount(hook *h, hook_f f);

int hook_call(hook h, ...);

#endif /* HOOK_H */
