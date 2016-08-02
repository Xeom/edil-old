#if !defined(IO_KEYMAP_H)
# define IO_KEYMAP_H
# include "io/key.h"
# include "container/vec.h"

typedef struct keymap_s keymap;

keymap *keymap_init(void);

void keymap_free(keymap *map);

void keymap_clear(keymap *map);

int keymap_press(keymap *map, key k);

hook *keymap_get_unknown(keymap *map);

hook *keymap_get(keymap *map, vec *keys);

int keymap_add(keymap *map, vec *keys);

int keymap_delete(keymap *map, vec *keys);

#endif /* IO_KEYMAP_H */
