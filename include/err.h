#if !defined(ERR_H)
# define ERR_H
# include "head.h"

# include <stddef.h>
# include <stdio.h>

typedef enum
{
    low,       /* Caused by a user, inconsequential e.g. Trying to delete a char that does not exist */
    medium,    /* Caused by a user, consequential   e.g. Unknown command                             */
    high,      /* Caused by bad high level code     e.g. Invalid parameters to function in config    */
    critical,  /* Should not happen                 e.g. Important object null                       */
    terminal,  /* Application cannot continue       e.g. Out of memory                               */
    errlvl_end
} err_lvl;

/* A string saying where an error is from */
#define ERRLOC " " __FILE__ ":" STRIFY(__LINE__)


#define ASSERT(code, level, fail)                       \
    do {                                                \
        if ( ! (code) )                                 \
        {                                               \
            err_new(level, "Assertion failed",          \
                    "(" #code ") returned 0" ERRLOC);   \
            {fail;}                                     \
        }                                               \
    } while (0)

#define ASSERT_PTR(code, level, fail)                   \
    do {                                                \
        if ( (code) == NULL )                           \
        {                                               \
            err_new(level, "Expected Non-NULL pointer", \
                    "(" #code ") returned NULL"         \
                    ERRLOC);                            \
            {fail;}                                     \
        }                                               \
    } while (0)

#define ASSERT_INT(code, level, fail)                   \
    do {                                                \
        if ( (int)(code) == -1 )                        \
        {                                               \
            err_new(level, "Expected 0 Return",         \
                    "(" #code ") returned non-zero"     \
                    ERRLOC);                            \
            {fail;}                                     \
        }                                               \
    } while (0)

#define ASSERT_IND(code, level, fail)                   \
    do {                                                \
        if ( (code) == INVALID_INDEX )                  \
        {                                               \
            err_new(level, "Invalid Index",             \
                    "(" #code ") returned Invalid Index"\
                    ERRLOC);                            \
            {fail;}                                     \
        }                                               \
    } while (0)

#define ASSERT_NCR(code, level, fail)                   \
    do {                                                \
        if ( (code) == ERR )                            \
        {                                               \
            err_new(level, "Ncurses error",             \
                    "(" #code ") returned ERR" ERRLOC); \
            {fail;}                                     \
        }                                               \
    } while(0)

#define TRACE(code, fail)     ASSERT    (code, err_last_lvl, fail)
#define TRACE_PTR(code, fail) ASSERT_PTR(code, err_last_lvl, fail)
#define TRACE_INT(code, fail) ASSERT_INT(code, err_last_lvl, fail)
#define TRACE_IND(code, fail) ASSERT_IND(code, err_last_lvl, fail)

/*
 * Reports a new error to the errsys
 *
 * level is an errlvl, not errlvl_end though.
 *
 * title is a quick overview of the error.
 *     It should start with the name of the function producing the error, followed by a colon
 *
 * details are any further relevant details of the error that may be useful to debug it.
 *     This may be NULL.
 *
 */
#define ERR_NEW(level, title, details) err_new(level, title, details ERRLOC)

extern FILE *err_stream;

extern uint max_err_per_second;

extern err_lvl err_min_quit_lvl;
extern err_lvl err_min_care_lvl;
extern err_lvl err_min_detail_lvl;

/* The level of the last error. Should not be changed. */
extern err_lvl err_last_lvl;

/*
 * Raises a new error to the error system
 *
 * level is an err_lvl, not errlvl_end though.
 *
 * title is a quick overview of the error.
 *     It should start with the name of the function producing the error, followed by a colon
 *
 * details are any further relevant details of the error that may be useful to debug it.
 *     This may be NULL.
 *
 */
void err_new(err_lvl lvl, const char *title, const char *details);

int err_create_log_buffer(void);

#endif /* ERR_H */
