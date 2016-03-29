#ifndef ERR_H
# define ERR_H
# include "head.h"

# include <stddef.h>

typedef enum
{
    low,       /* Caused by a user, inconsequential e.g. Trying to delete a char that does not exist */
    medium,    /* Caused by a user, consequential   e.g. Unknown command                             */
    high,      /* Caused by bad high level code     e.g. Invalid parameters to function in config    */
    critical,  /* Should not happen                 e.g. Important object null                       */
    terminal,  /* Application cannot continue       e.g. Out of memory                               */
    errlvl_end
} errlvl;

#define ERRLOC " " __FILE__ ":" STRIFY(__LINE__)

#define ASSERT(code, level, fail)                       \
    {                                                   \
        if ( ! (code) )                                 \
        {                                               \
            err_new(level, "Assertion failed",          \
                    #code " returneed 0" ERRLOC);       \
        }                                               \
    }

#define ASSERT_PTR(code, level, fail)                   \
    {                                                   \
        if ( (code) == NULL )                           \
        {                                               \
            err_new(level, "Expected Non-NULL pointer", \
                    #code " returned NULL" ERRLOC);     \
            {fail;}                                     \
        }                                               \
    }

#define ASSERT_INT(code, level, fail)                   \
    {                                                   \
        if ( (code) != 0 )                              \
        {                                               \
            err_new(level, "Expected 0 Return",         \
                    #code " returned non-zero"          \
                    ERRLOC);                          \
            {fail;}                                     \
        }                                               \
    }

#define ASSERT_IND(code, level, fail)                   \
    {                                                   \
        if ( (code) == INVALID_INDEX )                  \
        {                                               \
            err_new(level, "Invalid Index",             \
                    #code " returned Invalid Index"     \
                    ERRLOC);                          \
            {fail;}                                     \
        }                                               \
    }

#define ASSERT_NCR(code, level, fail)                   \
    {                                                   \
        if ( (code) == ERR )                            \
        {                                               \
            err_new(level, "Ncurses error",             \
                    #code " returned ERR" ERRLOC);    \
            {fail;}                                     \
        }                                               \
    }

#define TRACE_PTR(code, fail) ASSERT_PTR(code, err_last_lvl, fail)
#define TRACE_INT(code, fail) ASSERT_INT(code, err_last_lvl, fail)
#define TRACE_IND(code, fail) ASSERT_IND(code, err_last_lvl, fail)

#define ERR_NEW_STRIFY(level, title, details) err_new(level, #title, #details)

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

typedef struct err_s err;

/* Minimum error levels to quit, alert the user, and provide the user details of the error */
/* terminal, medium, and errlvl_end(never) by default. Can be changed at runtime           */
errlvl err_min_quit_lvl;
errlvl err_min_alert_lvl;
errlvl err_min_detail_lvl;

/* The level of the last error. Should not be changed. */
errlvl err_last_lvl;

/* Initialize the error system */
void err_initsys(void);

/* Kill the error system */
int err_killsys(void);

/*
 * Raises a new error to the error system
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
void err_new(errlvl level, const char *title, const char *details);

/*
 * Gets the most recent error and removes it from the error system. This function should be
 * used by something that is actively dealing with these errors, as they will not be recallable
 * from the error system in the future.
 *
 * Errors are returned by this function highest priority, oldest first. So first, all of the
 * highest priority errors are returned in chronological order, then the next highest priority etc.
 *
 * Returns NULL in the case that a most important error could not be found (due to internal error
 * or simply because there are no errors)(Yay)
 *
 * It is important to note that if this function returns NULL due to an error, there are still errors
 * in the system. However an internal error to this function is very bad, and means that something is
 * very, very fucked anyway. So just don't worry about that.
 */
err *err_pop(void);

/*
 * Returns the error level of an error.
 *
 * e is the error whose level we want
 *
 */
errlvl err_get_lvl(err *e);

/*
 * Returns the details of an error.
 *
 * e is the error whose details we want
 *
 */
const char *err_get_detail(err *e);

/*
 * Returns the title of an error.
 *
 * e is the error whose title we want
 *
 */
const char *err_get_title(err *e);

#endif /* ERR_H */
