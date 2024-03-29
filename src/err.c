#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "buffer/buffer.h"
#include "buffer/log.h"
#include "cursor/cursor.h"
#include "cursor/point.h"

#include "err.h"

char *err_lvl_prefixes[] =
{
    "[ low] ",
    "[ med] ",
    "[high] ",
    "[crit] ",
    "[term] ",
    ""
};

FILE   *err_stream     = NULL;
buffer *err_log_buffer = NULL;
cursor *err_log_cursor = NULL;

uint err_max_per_sec = 10;

err_lvl err_min_quit_lvl   = terminal;
err_lvl err_min_care_lvl   = low;
err_lvl err_min_detail_lvl = low;

err_lvl err_last_lvl;

static int err_are_we_dying(void);

static int err_are_we_dying(void)
{
    static uint last_err_time = 0;
    static uint errs_this_sec = 0;

    if ((uint)time(NULL) == last_err_time)
        errs_this_sec++;
    else
    {
        last_err_time = (uint)time(NULL);
        errs_this_sec = 1;
    }

    if (errs_this_sec == err_max_per_sec)
    {
        if (err_stream)
            fputs("Too many errors, ignoring further errors ...\n", err_stream);

        return 1;
    }

    if (errs_this_sec > err_max_per_sec)
        return 1;

    return 0;
}

int err_create_log_buffer(void)
{
    err_log_buffer = buffer_init();
    err_log_cursor = cursor_spawn(err_log_buffer, &cursor_point_type);
    err_stream     = buffer_log_point_stream(err_log_cursor);

    return 0;
}

void err_new(err_lvl lvl, const char *title, const char *content)
{
    char  *prefix;
    size_t plen, tlen, clen;
    static char *errstr = NULL;

//    char *a = NULL;
//    puts(a);

    err_last_lvl = lvl;

    if (lvl < err_min_care_lvl)
        return;

    if (err_are_we_dying())
        return;

    if (err_stream == NULL)
        err_stream = stderr;

    prefix = err_lvl_prefixes[lvl];

    plen = strlen(prefix);
    clen = strlen(content);
    tlen = strlen(title);

    errstr = realloc(errstr, plen + MAX(tlen, clen) + 1);

    memcpy(errstr, prefix, plen);
    memcpy(errstr + plen, title, tlen);

    errstr[plen + tlen] = '\n';

    if (err_stream)
    {
        fwrite(errstr, plen + tlen + 1, 1, err_stream);
        fflush(err_stream);
    }

    if (lvl < err_min_detail_lvl)
        return;

    memset(errstr, ' ', plen);
    memcpy(errstr + plen, content, clen);

    errstr[plen + clen    ] = '\n';

    if (err_stream)
    {
        fwrite(errstr, plen + clen + 1, 1, err_stream);
        fflush(err_stream);
    }

    if (lvl < err_min_quit_lvl)
        return;

    /* TODO: Graceful and less violent method of doing that */
    abort();
}
