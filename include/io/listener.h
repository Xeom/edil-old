#if !defined(IO_LISTENER_H)
# define IO_LISTENER_H
# include "head.h"
# include <stddef.h>
# include <stdio.h>

/* The listener system is designed to help edil handle listening to multiple  *
 * streams at once. Once created, a listener will call a certain function     *
 * whenever a certain file is ready to be read, as long as the                *
 * io_listener_listen function is called. The listener system also represents *
 * an attempt to contain nonblocking and polling io, which should aid in      *
 * devlopment of edil for multiple platforms.                                 */

typedef struct listener_s listener;

/* Types of functions that listeners can call */
typedef void (*listenf_none)(listener *);                 /* No data          */
typedef void (*listenf_char)(listener *, char);           /* Single character */
typedef void (*listenf_str )(listener *, char *, size_t); /* String and len   */

/* Default listener for stdin */
extern listener *stdin_listener;

/* Types of listener */
typedef enum
{
    read_char = 0x01, /* Read only single characters from the stream     */
    read_line = 0x02, /* Read sections of the line delimited by newlines */
    read_full = 0x03, /* Read all availiable characters                  */
    read_none = 0x04  /* Read nothing, just call listener function       */
} listen_type;

/*
 * Initialize the listener system.
 *
 * @return 0 on success, -1 on failure.
 *
 */
int io_listener_initsys(void);

/*
 * Initialize and return a pointer to a listener.
 *
 * The listener will listen to the provided stream, and be the specified
 * listen_type. If the listen_type is read_line or read_full, the stream will be
 * set into a nonblocking mode on linux. Every time a listener can be read from,
 * a specific function will be called.
 *
 * @param stream A file pointer to the stream for this listener to listen to.
 *
 * @param type   The listen_type this listener should have.
 *
 * @param limit  For read_full or read_line listen_types, the maximum number of
 *               characters to be read at once from the stream and handed to the
 *               function associated with this listener.
 *
 * @param charf  A function pointer for with read_char mode. Called with a
 *               pointer to the listener, and a single character. Should be NULL
 *               with listeners not in read_char mode.
 *
 * @param strf   A function pointer for read_full or read_line mode. Called with
 *               a pointer to the listener, a pointer to a string of characters,
 *               and the length of the string. Should be NULL with listeners not
 *               in read_full or read_line mode.
 *
 * @param nonef  A function ponter for read_none mode. Called only with a
 *               pointer to the listener. Should be NULL with listeners not in
 *               read_none mode.
 *
 * @return       A pointer to the new listener. NULL on failure.
 *
 */
listener *io_listener_init(FILE *stream, listen_type type, size_t limit,
                    listenf_char charf, listenf_str strf, listenf_none nonef);

/*
 * Frees a listener.
 *
 * @param li A pointer to the listener to free.
 *
 * @return   0 on success, -1 on failure.
 *
 */
int io_listener_free(listener *li);

/*
 * This is the function where edil spends most of its time.
 *
 * Polls all listener streams and waits until one is readable, then reads it and
 * calls the appropriate function. This function should be called by edil's main
 * loop.
 *
 * @return 0 on success, -1 on failure.
 *
 */
int io_listener_listen(void);

#endif /* IO_LISTENER_H */
