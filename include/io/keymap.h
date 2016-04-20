#if !defined(IO_KEYMAP_H)
# define IO_KEYMAP_H
# include "io/key.h"
# include "container/vec.h"

typedef struct keymap_s keymap;

keymap *keymap_init(void);

void keymap_clear(keymap *k);

int keymap_press(keymap *km, key k);

hook *keymap_get_unknown_hook(keymap *k);

hook *keymap_get(keymap *k, vec_keys *keys);

int keymap_add(keymap *k, vec_keys *keys);

int keymap_delete(keymap *k, vec *keys);

#endif /* IO_KEYMAP_H */
