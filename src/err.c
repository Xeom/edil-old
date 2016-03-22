#include <stdio.h>
#include <stdlib.h>

#include "container/vec.h"

#include "err.h"


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

    err_queue = vec_init(sizeof(vec *));

    if (err_queue == NULL)
        FUCK_VEC_ERR("err_init: Could not allocate err_queue main vector");

    i = errlvl_end;

    while (i--)
    {
        newv = vec_init(sizeof(err));

        if (newv == NULL)
            FUCK_VEC_ERR("err_init: Could not allocate err_queue sub-vector");

        if (vec_insert_end(err_queue, 1, &newv))
            FUCK_VEC_ERR("err_init: Could not push sub-vector into err_queue");
    }
}

int err_killsys(void)
{
    vec_foreach(err_queue, vec *, subqueue,
                vec_free(subqueue);
        );

    vec_free(err_queue);

    return 0;
}

void err_new(errlvl level, const char *title, const char *detail)
{
    vec **subptr, *subqueue;
    err  *e;

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


    subptr = (vec **)vec_item(err_queue, level);

    if (subptr == NULL)
        FUCK_VEC_ERR("Error getting pointer to subqueue");

    subqueue = *subptr;
    
    if (subqueue == NULL)
        FUCK_VEC_ERR("err_new_norecurse: Error getting error subqueue");

    if (vec_insert(subqueue, 0, 1, e))
        FUCK_VEC_ERR("err_new: Error pushing error to subqueue");
}

err *err_pop(void)
{
    errlvl level;
    vec *subqueue;
    err *rtn;

    level = errlvl_end;
    while (--level)
    {
        subqueue = *(vec **)vec_item(err_queue, level);

        if (subqueue == NULL)
        {
            err_new(critical, "err_get: Could not fetch error subqueue",
                    vec_err_str());
            return NULL;
        }

        if (vec_len(subqueue))
        {
            rtn = vec_item(subqueue, vec_len(subqueue) - 1);
            vec_delete(subqueue, vec_len(subqueue) - 1, 1);
            return rtn;
        }
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
