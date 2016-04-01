#if !defined(IO_H)
#define IO_H

typedef struct io_key_s io_key;

int io_initsys(void);

int io_killsys(void);

void io_ignore_curr(void);

char *io_key_str(io_key key);

int io_handle_chr(int chr);

#endif /* IO_H */
