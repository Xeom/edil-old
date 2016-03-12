/* YES */
#include "wincont.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct wincont_s
{
    textcont *text;
    wincont  *next;
    wincont  *prev;
    lineno    startline;
    colno     startcol;
};

wincont *wincont_root;

int wincont_initsys(void)
{
    wincont_root = NULL;
    wincont_root = wincont_init(textcont_init());

    wincont_root->prev = wincont_root;
    wincont_root->next = wincont_root;

    return 0;
}

wincont *wincont_init(textcont *text)
{
    wincont *rtn;

    rtn = malloc(sizeof(wincont));

    if (wincont_root)
    {
        rtn->next                = wincont_root;
        rtn->prev                = wincont_root->prev;
        rtn->next->prev          = rtn;
        rtn->prev->next          = rtn;
    }

    memset(rtn, 0, sizeof(wincont));

    rtn->text = text;

    return rtn;
}

wincont *wincont_clone(wincont *cont)
{
    wincont *rtn;

    rtn = wincont_init(cont->text);

    rtn->startline = cont->startline;
    rtn->startcol  = cont->startcol;

    return rtn;
}

wincont *wincont_next(wincont *cont)
{
    return cont->next;
}

wincont *wincont_prev(wincont *cont)
{
    return cont->prev;
}

const char *wincont_get_line_text_const(wincont *cont, line *l)
{
    const char *linetext;

    linetext = line_get_text_const(l);

    if (cont->startcol >= line_get_len(l))
        return "";
    else
        return linetext + cont->startcol;
}

line *wincont_get_line(wincont *cont, lineno ln)
{
    int   lineexists;
    line *rtn;
    lineno absln;

    absln = ln + cont->startline;

    lineexists = textcont_has_line(cont->text, absln);

    if (lineexists == 0)
    {
        ERR_NEW(high, "wincont_get_line: Invalid line",
                "textcont_has_line returned 0 - Line out of range of textcont");
        return NULL;
    }

    if (lineexists == -1)
        TRACE(wincont_get_line, textcont_has_line(cont->text, absln), NULL);

    rtn = textcont_get_line(cont->text, absln);
    TRACE_NULL(wincont_get_line, textcont_get_line(cont->txt, absln), rtn, NULL);

    return rtn;
}

wincont *wincont_get_root(void)
{
    return wincont_root;
}

textcont *wincont_get_textcont(wincont *cont)
{
    return cont->text;
}
