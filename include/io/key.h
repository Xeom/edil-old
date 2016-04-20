#if !defined(IO_KEY_H)
# define IO_KEY_H
# include "head.h"

# include "hook.h"

typedef struct key_s key;

#define IO_KEY_NAME_LEN 15

struct key_s
{
    char modifiers;
    char keyname[IO_KEY_NAME_LEN];
};

extern key io_key_incomplete;

extern const int io_key_resize;
extern const int io_key_name_len;

extern hook io_key_on_key;

key io_key_set_name(key k, const char *str);

int io_key_initsys(void);

int io_key_killsys(void);

char *io_key_str(key key);

int io_key_handle_chr(int chr);

#endif /* IO_KEY_H */
