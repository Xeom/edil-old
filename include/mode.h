#if !defined(MODE_H)
# define MODE_H
# include "head.h"

# include "io/keymap.h"
# include "io/key.h"

typedef struct mode_s mode;

extern hook mode_on_free;
extern hook mode_on_init;

mode *mode_init(int priority, char *name);

int mode_activate(mode *m);

int mode_deactivate(mode *m);

hook *mode_get_on_activate(mode *m);

hook *mode_get_on_deactivate(mode *m);

keymap *mode_get_keymap(mode *m);

int mode_handle_press(key k);

int mode_free(mode *m);

int mode_add_mount(mode *m, hook *h, hook_f f);

int mode_remove_mount(mode *m, hook *h, hook_f f);

#endif /* MODE_H */
