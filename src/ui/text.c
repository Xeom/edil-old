#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>

#include <stdint.h>

#include "err.h"

#include "ui/text.h"

text_symbol_type ui_text_symbol(char c)
{
    /* Special case - \n is an escape for a face. */
    if ((uchar)c == '\n')
        return face_start;

    /* 0xxx xxxx - Ordinary ascii */
    if ((uchar)c < 0x80)
        return ascii;

    /* 10xx xxxx - utf-8 continuation character */
    if ((uchar)c < 0xc0)
        return utf8_mid;

    /* 110x xxxx - utf-8 2-byte wide start character */
    if ((uchar)c < 0xe0)
        return utf8_2wide;

    /* 1110 xxxx - utf-8 3-byte wide start character */
    if ((uchar)c < 0xf0)
        return utf8_3wide;

    /* 1111 xxxx - utf-8 4-byte wide start character */
    if ((uchar)c < 0xf8)
        return utf8_4wide;

    /* Anything else is a utf-8 starting character for more than 4 bytes */
    return utf8_big;
}

size_t ui_text_symbol_width(text_symbol_type sym)
{
    switch (sym)
    {
    case face_start:
        return face_serialized_len;
    case utf8_big:
    case utf8_mid:
        return 1;
    default:
        return (size_t)sym;
    }
}

int ui_text_symbol_is_char(text_symbol_type sym)
{
    return sym <= utf8_4wide;
}

char *ui_text_next_symbol(const char *str, const char *end)
{
    char *rtn;

    if (str >= end) return NULL;

    rtn = (char *)str + ui_text_symbol_width(ui_text_symbol(*str));

    if (rtn >= end) return NULL;

    return rtn;
}

int32_t ui_text_decode_utf8(const char *str, const char *end)
{
    text_symbol_type typ;
    int32_t          rtn;
    size_t           width;

    ASSERT_PTR(str, high, return -1);
    ASSERT_PTR(end, high, return -1);
    ASSERT(end >= str, high, return -1);

    typ = ui_text_symbol(*str);

    width = ui_text_symbol_width(typ);

    ASSERT(str + width <= end,          high, return -1);
    ASSERT(ui_text_symbol_is_char(typ), high, return -1);

    if (typ == ascii)
        return (int32_t)*str;

    rtn = (0x7f >> (int)typ) & (*str);

    while (--width)
    {
        rtn <<= 6;
        rtn  |= *(++str) & 0x3f;
    }

    return rtn;
}

size_t ui_text_encode_utf8(int32_t chr, char *buf)
{
    ASSERT(chr < 0, high, return 0);

    if (chr < 0x80)
    {
        *buf = (char)chr;

        return 1;
    }

    else if (chr < 0x800)
    {
        buf[0] = (char)(0xc0 | (chr >> 6));
        buf[1] = (char)(0x80 | (chr & 0x3f));

        return 2;
    }

    else if (chr < 0x10000)
    {
        buf[0] = (char)(0xe0 | (chr >> 12));
        buf[1] = (char)(0x80 | ((chr >> 6) & 0x3f));
        buf[2] = (char)(0x80 | (chr & 0x3f));

        return 3;
    }

    else if (chr < 0x400000)
    {
        buf[0] = (char)(0xe0 | (chr >> 18));
        buf[1] = (char)(0x80 | ((chr >> 12) & 0x3f));
        buf[2] = (char)(0x80 | ((chr >>  6) & 0x3f));
        buf[3] = (char)(0x80 | (chr & 0x3f));

        return 4;
    }

    return 0;
}

char *ui_text_next_char(const char *str, const char *end)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return NULL);

    while ((str = ui_text_next_symbol(str, end)))
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            return (char *)str;
    }

    return NULL;
}

char *ui_text_first_char(const char *str, const char *end)
{
    text_symbol_type typ;

    if (str == end)
        return NULL;

    typ = ui_text_symbol(*str);

    if (ui_text_symbol_is_char(typ))
        return (char *)str;

    else
        return ui_text_next_char(str, end);
}


char *ui_text_next_face(const char *str, const char *end)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return *(char **)NULL);

    while ((str = ui_text_next_symbol(str, end)))
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (typ == face_start)
            return (char *)str;
    }

    return NULL;
}

char *ui_text_first_face(const char *str, const char *end)
{
    text_symbol_type typ;

    if (str == end)
        return NULL;

    typ = ui_text_symbol(*str);

    if (typ == face_start)
        return (char *)str;

    else
        return ui_text_next_face(str, end);
}

char *ui_text_get_char(const char *str, const char *end, size_t n)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return NULL);
    ASSERT(str < end, high, return NULL);

    while (str)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            if (n-- == 0)
                return (char *)str;

        str = ui_text_next_symbol(str, end);
    }

    if (n == 0) return (char *)end;

    return NULL;
}

size_t ui_text_len(const char *str, const char *end)
{
    size_t rtn;

    ASSERT_PTR(str, high, return 0);
    ASSERT_PTR(end, high, return 0);

    rtn = 0;

    while (str < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            rtn++;

        str = ui_text_next_symbol(str, end);
    }

    return rtn;
}

short ui_text_face_overflow(const char *str, const char *end, face *f)
{
    short facen;
    char *facechar, *textchar, *nextface;

    ASSERT_PTR(str, high, return -1);
    if (end == NULL) end = str + strlen(str);

    facen = 0;

    facechar = ui_text_first_face(str, end);

    if (!facechar)
        return 0;

    while ((nextface = ui_text_next_face(facechar, end)))
        facechar = nextface;

    textchar = facechar;

    if (f) *f = ui_face_deserialize_face(facechar);
    facen     = ui_face_deserialize_length(facechar);

    while (facen && (textchar = ui_text_next_char(textchar, end)))
        facen--;

    return facen;
}

int ui_text_draw_h(
    const char *str,
    const char *end,
    uint sizelim,
    chtype filler,
    short facen)
{
    char *facechar, *textchar;

    if (str == NULL)
    {
        facechar = NULL;
        textchar = NULL;
    }
    else
    {
        if (end == NULL) end = str + strlen(str);

        facechar = ui_text_first_face(str, end);
        textchar = ui_text_first_char(str, end);
    }

    while (--sizelim)
    {
        while (facechar && facen == 0 && facechar < textchar)
        {
            if (facechar + face_serialized_len <= end)
            {
                face f;
                f        = ui_face_deserialize_face(facechar);
                facen    = ui_face_deserialize_length(facechar);
                attron(ui_face_get_attr(f));

                facechar = ui_text_next_face(facechar, end);
            }
            else
                facechar = NULL;
        }

        if (textchar)
        {
            int32_t chrint;
            wchar_t chr;

            chrint   = ui_text_decode_utf8(textchar, end);
            chr      = (wchar_t)((chrint != -1) ? chrint : L'?');

            if (facen == 0) attrset(0);
            else facen--;

            addnwstr(&chr, 1);

            textchar = ui_text_next_char(textchar, end);
        }
        else
            addch(filler);

    }

    return 0;
}

