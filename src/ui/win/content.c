#include <curses.h>
#include <string.h>

#include "buffer/buffer.h"
#include "win/win.h"
#include "win/iter.h"
#include "win/pos.h"
#include "win/size.h"
#include "ui/win/win.h"
#include "ui/util.h"

#include "ui/win/content.h"
#include "ui/face.h"
#include "ui/text.h"

#include "err.h"

size_t ui_win_content_get_cursor_offset(win *w, lineno ln)
{
    buffer *b;
    vec    *l;
    char   *str, *end;
    size_t  col;

    ASSERT_PTR(b = win_get_buffer(w), high, return INVALID_INDEX);

    ASSERT(ln < buffer_len(b), high, return INVALID_INDEX);

    ASSERT_PTR(l = buffer_get_line(b, ln), high, return INVALID_INDEX);

    hook_call(ui_win_content_on_draw_line_pre, w, b, &ln, l);

    if (vec_len(l) == 0)
        return INVALID_INDEX;

    str = (char *)vec_item(l, 0);
    end = (char *)vec_item(l, vec_len(l) - 1) + 1;

    col = 0;

    while (str && str < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (typ == face_start &&
            memcmp(str, face_cursor, face_serialized_len) == 0)
        {
            vec_free(l);
            return col;
        }

        if (ui_text_symbol_is_char(typ))
            col++;

        str = ui_text_next_symbol(str, end);
    }

    vec_free(l);

    return INVALID_INDEX;
}

int ui_win_content_draw_subs(win *w)
{
    win *sub, *first;

    first = win_iter_first(w);
    sub   = first;

    do
    {
        ui_win_content_draw(sub);
        sub = win_iter_next(sub);
    } while (sub != first);

    return 0;
}

int ui_win_content_draw(win *w)
{
    ulong curr, max;

    curr = win_get_offsety(w);
    max  = (ulong)win_size_get_y(w) + curr;

    for (; curr < max; ++curr)
        ui_win_content_draw_line(w, curr);

    return 0;
}

int ui_win_content_draw_lines_after(win *w, lineno ln)
{
    ulong curr, max;

    curr = MAX(ln, win_get_offsety(w));
    max  = win_size_get_y(w) + ln;

    for (; curr < max; curr++)
        ui_win_content_draw_line(w, curr);

    return 0;
}

int ui_win_content_draw_line(win *w, lineno ln)
{
    buffer *b;
    vec    *l;
    size_t  numlines;
    uint    sizex, sizey;
    ulong   offx,  offy;
    int     currx, curry;

    b        = win_get_buffer(w);
    numlines = buffer_len(b);

    offx = win_get_offsetx(w);
    offy = win_get_offsety(w);

    curry = win_pos_get_y(w) + (int)(ln - offy);
    currx = win_pos_get_x(w);

    sizex = win_size_get_x(w);
    sizey = win_size_get_y(w);

    if (ln >= offy + sizey - 1 || ln < offy)
        return 0;

    if (ln >= numlines)
        l = NULL;
    else
        l = buffer_get_line(b, ln);

    if (l)
    {
        hook_call(ui_win_content_on_draw_line_pre, w, b, &ln, l);

        if (vec_len(l) == 0)
            l = NULL;
    }

    move(curry++, currx);

    if (l)
    {
        char *start, *str, *end;
        face   startface;
        short  startfacen;

        start = (char *)vec_item(l, 0);
        end   = (char *)vec_item(l, vec_len(l) - 1) + 1;

        str = ui_text_get_char(start, end, offx);

        if (str == NULL) str = end;

        startface  = face_default;
        startfacen = ui_text_face_overflow(start, str, &startface);

        attron(ui_face_get_attr(startface));

        ui_text_draw_h(str, end, sizex - 1, ' ', startfacen);
    }
    else
        ui_text_draw_h(NULL, NULL, sizex - 1, ' ', 0);

    hook_call(ui_win_content_on_draw_line_post, w, b, &ln, l);

    vec_free(l);

    return 0;
}
