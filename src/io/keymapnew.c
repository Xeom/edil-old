#include <stdlib.h>

#include "io/key.h"
#include "container/table.h"
#include "hook.h"

#include "io/keymapnew.h"

hook_add(keymap_on_free, 1);
hook_add(keymap_on_init, 1);

typedef struct ktree_s ktree;

typedef enum
{
    kmap,
    leaf
} ktree_type;

struct ktree_s
{
    union
    {
        table *subs;
        hook h;
    } cont;
    ktree_type type;
    int        refs;
}

    
