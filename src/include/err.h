#ifndef ERR_H
#define ERR_H
#include <stddef.h>

typedef enum
{
    low,       /* Caused by a user, inconsequential e.g. Trying to delete a char that does not exist */
    medium,    /* Caused by a user, consequential   e.g. Unknown command                             */
    high,      /* Caused by bad high level code     e.g. Invalid parameters to function in config    */
    critical,  /* Should not happen                 e.g. Important object null                       */
    terminal,  /* Application cannot continue       e.g. Out of memory                               */
    errlvl_end
} errlvl;

#define ERRLOC Line: __LINE__ File: __FILE__

/* we need this since # formatting only works on parameters, so we can't do it to ERRLOC */
#define ERR_NEW_CALL(level, title, details) err_new(level, #title, #details)

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
#define ERR_NEW(level, title, details) ERR_NEW_CALL(level, title, details ERRLOC)

/*
 * Checks that var is not NULL. If it is, throws a high level error stating that funct was
 * handed var as a parameter, and it was null. The macro then returns rtn from the function 
 * using this macro. A useful shortcut for ERR_NEW.
 *
 * funct is the name of the function using the macro.
 *
 * var is the variable to check for equality with NULL.
 *
 * rtn is the value to return from the function using this macro if var is NULL.
 *
 */
#define CHECK_NULL_PRM(funct, var, rtn)                      \
    if (var == NULL) {                                       \
        ERR_NEW(high,                                        \
                funct  : NULL argument, Argument name: var); \
        return rtn;                                          \
    }

/*
 * Checks that var is not NULL. If it is, throws a terminal level error stating that the
 * application is out of memory. The macro then returns rtn from the function using this macro.
 * (Though that's not particularly useful, is it...) A useful shortcut for ERR_NEW.
 *
 * funct is the name of the function using this macro.
 *
 * var is the variable whose value to check for equality with NULL.
 *
 * rtn is the value to return from the function using this macro if var is NULL.
 *
 */
#define CHECK_ALLOC(funct, var, rtn)                \
    if (var == NULL) {                              \
        ERR_NEW(terminal,                           \
                funct : Out of memory,              \
                Could not allocate memory for var); \
        return rtn;                                 \
    }

/*
 * If var is non-zero, raises an error of the same level as the last error raised, stating that
 * call had an error while being called from funct. The macro then returns rtn from the function
 * using the macro. Used for producing pseudo-tracebacks of errors. NOTE: Proper tracebacks may
 * be implmented in the future.
 *
 * funct is the name of the function using this macro.
 *
 * call is the function and parameters that produced var's value.
 *
 * var is the variable whose value to check for equality with zero.
 *
 * rtn is the value to return from the function using this macro if var is nonzero.
 */
#define TRACE_NONZRO(funct, call, var, rtn)         \
    if (var) {                                      \
        ERR_NEW(err_last_lvl,                       \
                funct : Call call failed,           \
                call returned nonzero return code); \
        return rtn;                                 \
    }

/*
 * Perform call, and if it returns a nonzero value, use TRACE_NONZERO with call as the call
 * and var.
 *
 * funct is the name of the function using this macro.
 *
 * call is the function to call and check the return value of.
 *
 * rtn is the value to return from the function using this macro if var is NULL.
 */
#define TRACE_NONZRO_CALL(funct, call, rtn) TRACE_NONZRO(funct, call, call, rtn)

/*
 * If var is NULL, raises an error of the same level as the last error raised, stating that
 * call had an error while being called from funct. The macro then returns rtn from the function
 * using the macro. Used for producing pseudo-tracebacks of errors. NOTE: Proper tracebacks may
 * be implmented in the future.
 *
 * funct is the name of the function using this macro.
 *
 * call is the function and parameters that produced var's value.
 *
 * var is the variable whose value to check for equality with NULL.
 *
 * rtn is the value to return from the function using this macro if var is NULL.
 */
#define TRACE_NULL(funct, call, var, rtn)                               \
    if (var == NULL) {                                                  \
        ERR_NEW(err_last_lvl,                                           \
                funct : Call call failed,                               \
                call returned nonzero return code ERRLOC);              \
        return rtn;                                                     \
    }

typedef struct err_s err;

/* Minimum error levels to quit, alert the user, and provide the user details of the error */
/* terminal, medium, and errlvl_end(never) by default. Can be changed at runtime           */
errlvl err_min_quit_lvl;
errlvl err_min_alert_lvl;
errlvl err_min_detail_lvl;

/* The level of the last error. Should not be changed. */
errlvl err_last_lvl;

/* Initialize the error system */
void errsys_init(void);

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
 * Gets the i-th error of the specified level. Lower values of i are more recent.
 *
 * Returns NULL in the case that the error could not be found (due to internal error or 
 * simply because it does not exist)
 *
 * level is the level of the error to get.
 *
 * i is the index of the error to get.
 *
 */
err *err_get(errlvl level, size_t i);

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
const char *err_get_details(err *e);

/*
 * Returns the title of an error.
 *
 * e is the error whose title we want
 *
 */
const char *err_get_title(err *e);

#endif /* ERR_H */
