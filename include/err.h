#if !defined(ERR_H)
# define ERR_H
# include "head.h"

# include <stddef.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>

# include "buffer/buffer.h"
# include "cursor/cursor.h"

/* Different levels of severity of errors. */
typedef enum
{
    low,       /* Caused by a user, inconsequential                *
                * e.g. Trying to delete a char that does not exist */

    medium,    /* Caused by a user, consequential                  *
                * e.g. Unknown command                             */

    high,      /* Caused by bad high level code                    *
                * e.g. Invalid parameters to function in config    */

    critical,  /* Should not happen                                *
                * e.g. Important object null                       */

    terminal,  /* Application cannot continue                      *
                * e.g. Out of memory                               */

    errlvl_end /* Used only to get max value of enum. */
} err_lvl;


/* A definition that resolves to the current file and linenumber. *
 * Used to mark where errors came from.                           */
#define ERRLOC " " __FILE__ ":" STRIFY(__LINE__)

/*
 * Assert that something is nonzero.
 *
 * Ensures that a statement is not equal to zero, and raises an error of a
 * specified level, then runs some fail code if the statement is equal to zero.
 *
 * @param code  The statement to evaluate.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
#define ASSERT(code, level, fail)                       \
    do {                                                \
        if ( ! (code) )                                 \
        {                                               \
            err_new(level, "Assertion failed",          \
                    "(" #code ") returned 0" ERRLOC);   \
            {fail;}                                     \
        }                                               \
    } while (0)

/*
 * Assert that a pointer is non-null.
 *
 * While roughly equivilent to ASSERT, this assertion produces an error message
 * clearly stating that the error was due to a null pointer.
 *
 * @param code  The statement to evaluate to produce a pointer.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
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


/*
 * Assert that a statement is not -1.
 *
 * By convention, many edil calls return -1 on error. It is therefore useful to
 * be able to execute a call and check that the result is not -1 in a simple
 * assertion.
 *
 * @param code  The statement to evaluate.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
#define ASSERT_INT(code, level, fail)                   \
    do {                                                \
        if ( (int)(code) == -1 )                        \
        {                                               \
            err_new(level, "Unexpected -1 return.",     \
                    "(" #code ") returned -1"           \
                    ERRLOC);                            \
            {fail;}                                     \
        }                                               \
    } while (0)

/*
 * Assert that a statement does not evaluate to INVALID_INDEX.
 *
 * Some edil calls returning unsigned longs as indices will return INVALID_INDEX
 * on error. This will generally be equal to -1, but this cannot be guarenteed,
 * so it is useful to have an assertion that checks the validity of indices.
 *
 * @param code  The statement to evaluate.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
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

/*
 * Assert that a call does not return ERR.
 *
 * Many NCurses calls return ERR on failure, so an assertion to check that
 * values are not equal to ERR is useful. ERR is generally -1, like ASSERT_IND
 * and ASSERT_INT, but again, this cannot be guarenteed, and the different
 * assertions make it clearer the type of error.
 *
 * @param code  The statement to evaluate.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
#define ASSERT_NCR(code, level, fail)                   \
    do {                                                \
        if ( (code) == ERR )                            \
        {                                               \
            err_new(level, "Ncurses error",             \
                    "(" #code ") returned ERR" ERRLOC); \
            {fail;}                                     \
        }                                               \
    } while(0)

/*
 * Assert that a statment is nonzero, and raise an errno error otherwise.
 *
 * This statement is generally used where a system function could fail. While it
 * checks for nonzeroness in an identical way to ASSERT, it sets the error title
 * to a string describing errno.
 *
 * @param code  The statement to evaluate.
 * @param level The level of the error to raise. Should be an err_lvl.
 * @param fail  The code to run if the assertion fails.
 *
 */
#define ASSERT_ENO(code, level, fail)                   \
    do {                                                \
        if ( ! (code) )                                 \
        {                                               \
            err_new(level,  strerror(errno),            \
                    "(" #code ") returned 0.");         \
            {fail;}                                     \
        }                                               \
    } while (0)

/*
 * These trace functions are identical to their ASSERT_* counterparts. The
 * difference however, is that they do not take a level parameter. They assume
 * the error level of the failure is the same as the last error.
 *
 * In the future, these may be changed to explicitly produce tracebacks from
 * errors.
 *
 */
#define TRACE(code, fail)     ASSERT    (code, err_last_lvl, fail)
#define TRACE_PTR(code, fail) ASSERT_PTR(code, err_last_lvl, fail)
#define TRACE_INT(code, fail) ASSERT_INT(code, err_last_lvl, fail)
#define TRACE_IND(code, fail) ASSERT_IND(code, err_last_lvl, fail)
#define TRACE_NCR(code, fail) ASSERT_NCR(code, err_last_lvl, fail)
#define TRACE_ENO(code, fail) ASSERT_ENO(code, err_last_lvl, fail)

/*
 * Reports a new error to the error system.
 *
 * The error has a title, which should be a general description of the type of
 * error it is, and a description, which should be specific about where the
 * error happened. The lineno and filename where ERR_NEW was called from are
 * appended to the details of the error. Use err_new if this is not desired.
 *
 * details and title should be literal strings. Use err_new if modifying them is
 * required.
 *
 * @level   The err_lvl of the new error.
 * @title   The title of the new error.
 * @details The details string of the new error.
 *
 */
#define ERR_NEW(level, title, details) err_new(level, title, details ERRLOC)

/* A file stream where details of errors are reported.    *
 * NULL will disable printing details of errors anywhere. */
extern FILE *err_stream;

/* Pointers to the error buffer and cursor used for logging errors if *
 * err_create_log_buffer has been called. NULL otherwise.             */
extern buffer *err_log_buffer;
extern cursor *err_log_cursor;


/* The maximum number of errors that should be processed every second *
 * before the error system starts ignoring them.                      */
extern uint err_max_per_sec;

/* The minimum error level that results in the application aborting. */
extern err_lvl err_min_quit_lvl;
/* The minimum error level that the error system cares at all about. */
extern err_lvl err_min_care_lvl;
/* The minimum error level that the error system prints details of *
 * to err_stream.                                                  */
extern err_lvl err_min_detail_lvl;

/* The level of the last error. */
extern err_lvl err_last_lvl;

/*
 * Reports a new error to the error system.
 *
 * The error has a title, which should be a general description of the type of
 * error it is, and a description, which should be specific about where the
 * error happened.
 *
 * @level   The err_lvl of the new error.
 * @title   The title of the new error.
 * @details The details string of the new error.
 *
 */
void err_new(err_lvl lvl, const char *title, const char *details);

/*
 * Creates a new buffer and points err_stream at it.
 *
 * The buffer will then fill up with details of edil's errors, allowing for easy
 * debugging.
 *
 */
int err_create_log_buffer(void);

#endif /* ERR_H */
