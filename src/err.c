#include "err.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>

struct err_s
{
    const char *title;
    const char *detail;
    errlvl lvl;
};

#define FUCK_IT(err) {fputs("\n", stderr); fputs(err, stderr); fputs("\nFuck it.\n", stderr); abort();}
#define FUCK_VEC_ERR(err) {fputs("\n", stderr); fputs(err, stderr); FUCK_IT(vec_err_str());}

/*
    [
        &[{char* title, char* detail, errlvl lvl}, ...low priority errors],
        &[...medium priority errors],
        &[...high priority errors],
         ...all other error lvls
    ]
*/
vec *err_queue;

void err_new_norecurse(errlvl level, const char *title, const char *detail);

void err_initsys(void)
{
    errlvl i;
    vec   *newv;

    err_min_quit_lvl   = terminal;
    err_min_alert_lvl  = medium;
    err_min_detail_lvl = errlvl_end;

    err_queue = vec_init(sizeof(vec));

    if (err_queue == NULL)
        FUCK_VEC_ERR("err_init: Could not allocate err_queue main vector");

    i = errlvl_end;

    newv = vec_init(sizeof(err));

    if (newv == NULL)
        FUCK_VEC_ERR("err_init: Could not allocate err_queue sub-vector");

    while (i--)
    {
        if (vec_push(err_queue, newv))
            FUCK_VEC_ERR("err_init: Could not push sub-vector into err_queue");
    }
    free(newv);
}

void err_new_norecurse(errlvl level, const char *title, const char *detail)
{
    vec *subqueue;
    err *e;

    e = malloc(sizeof(err));

    if (e == NULL)
        FUCK_IT("err_new_norecurse: Could not allocate memory for error");

    if (title == NULL)
        title = "Unknown error";

    e->title = title;

    if (detail == NULL)
        detail = "";

    e->detail = detail;

    if (level >= errlvl_end)
    {
        level = high;
    }

    subqueue = vec_get(err_queue, level);

    if (subqueue == NULL)
        FUCK_VEC_ERR("err_new_norecurse: Error getting error subqueue");

    if (vec_insert(subqueue, 0, e))
        FUCK_VEC_ERR("err_new: Error pushing error to subqueue");
}

void err_new(errlvl level, const char *title, const char *detail)
{
    vec *subqueue;
    err *e;

    e = malloc(sizeof(err));

    if (e == NULL)
        FUCK_IT("err_new: Could not allocate memory for error");

    if (title == NULL)
        title = "Unknown error";

    e->title = title;

    if (detail == NULL)
        detail = "";

    e->detail = detail;

    if (level >= errlvl_end)
    {
        err_new_norecurse(high, "err_new: Invalid error level",
                          "handed unknown (too high) error level");
        level = high;
    }

    subqueue = vec_get(err_queue, level);

    err_last_lvl = level;

    if (subqueue == NULL)
        err_new_norecurse(critical, "err_new: Error getting error subqueue",
                          vec_err_str());

    if (vec_insert(subqueue, 0, e))
        err_new_norecurse(critical, "err_new: Error pushing error to subqueue",
                          vec_err_str());
}

err *err_get(errlvl level, size_t i)
{
    vec *subqueue;
    err *rtn;

    subqueue = vec_get(err_queue, level);

    if (subqueue == NULL)
    {
        err_new(critical, "err_get: Could not fetch error subqueue",
                vec_err_str());
        return NULL;
    }

    if (i >= vec_len(subqueue))
        return NULL;

    rtn = vec_get(subqueue, vec_len(subqueue) - i - 1);

    if (rtn == NULL)
        err_new(critical, "err_get: Error getting err* from subqueue",
                vec_err_str());

    return rtn;
}

err *err_pop(void)
{
    errlvl level;
    vec *subqueue;

    level = errlvl_end;
    while (--level)
    {
        subqueue = vec_get(err_queue, level);

        if (subqueue == NULL)
        {
            err_new(critical, "err_get: Could not fetch error subqueue",
                    vec_err_str());
            return NULL;
        }

        if (vec_len(subqueue))
            return vec_pop(subqueue);
    }

    return NULL;
}

const char *err_get_detail(err *e)
{
    if (e == NULL)
        return NULL;

    return e->detail;
}


const char *err_get_title(err *e)
{
    if (e == NULL)
        return NULL;

    return e->title;
}
