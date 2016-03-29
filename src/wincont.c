#include <stdlib.h>
#include <string.h>

#include "err.h"

#include "wincont.h"
#include "buffer/core.h"

struct wincont_s
{
    buffer   *buf;
    wincont  *next;
    wincont  *prev;
    lineno    startline;
    colno     startcol;
};

wincont *wincont_root;

int wincont_initsys(void)
{
    wincont_root = NULL;
    TRACE_INT(wincont_root = wincont_init(buffer_init()),
              return -1);

    wincont_root->prev = wincont_root;
    wincont_root->next = wincont_root;

    return 0;
}

int wincont_killsys(void)
{
    wincont *cont;

    cont = wincont_root;
    do
    {
        free(cont);/* Implement real free */
        cont = cont->next;
    }
    while (cont != wincont_root);

    return 0;
}

wincont *wincont_init(buffer *b)
{
    wincont *rtn;

    ASSERT_PTR(rtn = malloc(sizeof(wincont)), terminal,
               return NULL);

    if (wincont_root)
    {
        rtn->next                = wincont_root;
        rtn->prev                = wincont_root->prev;
        rtn->next->prev          = rtn;
        rtn->prev->next          = rtn;
    }

    memset(rtn, 0, sizeof(wincont));

    rtn->buf = b;

    return rtn;
}

wincont *wincont_clone(wincont *cont)
{
    wincont *rtn;

    ASSERT_PTR(cont, high, return NULL);

    TRACE_PTR(rtn = wincont_init(cont->buf),
              return NULL);

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

wincont *wincont_get_root(void)
{
    return wincont_root;
}

buffer *wincont_get_buffer(wincont *cont)
{
    ASSERT_PTR(cont, high,
               return NULL);

    return cont->buf;
}
