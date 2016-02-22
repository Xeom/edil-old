#include "err.h"
#include "vec.h"

struct err_s
{
	char *title;
	char *detail;
	errlvl lvl;
}

#define FUCK_IT(err) {fputs("\n", stderr); fputs(err, stderr); fputs("\nFuck it.\n", stderr); abort();}

/*
    [
        &[{char* title, char* detail, errlvl lvl}, ...low priority errors],
        &[...medium priority errors],
        &[...high priority errors],
         ...all other error lvls
    ]
*/
vec *err_queue;

void fuck_vec_err(const char *desc)
{
	fputs("\n", stderr);
	fputs(desc, stderr);
	FUCK_IT(vec_err_str());
}

void errsys_init(void)
{
	err_min_quit_lvl   = terminal;
	err_min_alert_lvl  = medium;
	err_min_detail_lvl = errlvl_end;

	err_queue = vec_init(sizeof(vec*));

	if (err_queue == NULL)
		fuck_vec_err("err_init: Could not allocate err_queue main vector");

	errlvl i = errlvl_end;

	while (--i)
	{
		vec *newv = vec_init(sizeof(err));

		if (newv == NULL)
			fuck_vec_err("err_init: Could not allocate err_queue sub-vector");

		if (vec_push(err_queue, newv))
			fuck_vec_err("err_init: Could not push sub-vector into err_queue");
	}
}

void err_new(errlvl level, const char *title, const char *detail)
{
	err = malloc(sizeof(err));

	if (err == NULL)
		FUCK_IT("err_new: Could not allocate memory for error");

	if (title == NULL)
		title = "Unknown error";

	err->title = title;

	if (detail == NULL)
		detail = "";

	err->detail = detail;

	if (level >= errlvl_end)
	{
		err_new_norecurse(high, "err_new: Invalid error level",
						  "handed unknown (too high) error level");
		level = high;
	}

	vec *subqueue = vec_get(err_queue, level);

	if (subqueue == NULL)
		err_new_norecurse(critical, "err_new: Error getting error subqueue",
						  vec_err_str());

	if (vec_push(subqueue, err))
		err_new_norecurse(critical, "err_new: Error pushing error to subqueue",
						  vec_err_str());
}

void err_new_norecurse(errlvl level, const char *title, const char *detail)
{
	err = malloc(sizeof(err))

	if (err == NULL)
		FUCK_IT("err_new_norecurse: Could not allocate memory for error");

	if (title == NULL)
		title = "Unknown error";

	err->title = title;

	if (detail == NULL)
		detail = "";

	err->detail = detail;

	if (level >= errlvl_end)
	{
		level = high;
	}

	vec *subqueue = vec_get(err_queue, level);

	if (subqueue == NULL)
		fuck_vec_err("err_new_norecurse: Error getting error subqueue");

	if (vec_push(subqueue, err))
		fuck_vec_err("err_new: Error pushing error to subqueue");
}

err *err_get(err *e, errlvl level, size_t i)
{
	vec *subqueue = vec_get(err_queue, level);

	if (subqueue == NULL)
	{
		err_new(critical, "err_get: Could not fetch error subqueue",
				vec_err_str());
		return NULL;
	}

	if (i >= vec_len(subqueue))
		return NULL;

	err *rtn = vec_get(subqueue, vec_len(subqueue) - i - 1);

	if (rtn == NULL)
		err_new(critical, "err_get: Error getting err* from subqueue",
				vec_err_str());

	return rtn;
}

err *err_pop(err *e)
{
	errlvl level = errlvl_end;

	while (--level)
	{
		vec *subqueue = vec_get(err_queue, level);

		if (subqueue == NULL)
		{
			err_new(critical, "err_get: Could not fetch error subqueue",
					vec_err_str());
			return NULL;
		}

		
	

