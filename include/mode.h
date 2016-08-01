#if !defined(MODE_H)
# define MODE_H
# include "head.h"

# include "io/key.h"

typedef struct mode_s mode;

mode *mode_init(int priority, char *name);

int mode_activate(mode *m);

int mode_deactivate(mode *m);

int mode_handle_press(key k);

int mode_free(mode *m);

#endif /* MODE_H */
