#include <stdint.h>

#include "ui/face.h"
#include "err.h"

#include "ui/text.h"

text_symbol_type ui_text_symbol(char c)
{
    if ((uchar)c == '\n')
        return face_start;

    if ((uchar)c < 0x80)
        return ascii;

    if ((uchar)c < 0xc0)
        return utf8_mid;

    if ((uchar)c < 0xe0)
        return utf8_2wide;

    if ((uchar)c < 0xf0)
        return utf8_3wide;

    if ((uchar)c < 0xf8)
        return utf8_4wide;

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

char *ui_text_next_symbol(char *str)
{
    return str + ui_text_symbol_width(ui_text_symbol(*str));
}

int ui_text_symbol_is_char(text_symbol_type sym)
{
    return sym <= utf8_4wide;
}

char *ui_text_next_char(char *str, char *end)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return NULL);

    while (str < end &&
           (str = ui_text_next_symbol(str)) < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            return str;
    }

    return NULL;
}

char *ui_text_next_face(char *str, char *end)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return NULL);

    while (str < end &&
           (str = ui_text_next_symbol(str)) < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (typ == face_start)
            return str;
    }

    return NULL;
}

size_t ui_text_len(char *str, char *end)
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

        str = ui_text_next_symbol(str);
    }

    return rtn;
}

char *ui_text_get_char(char *str, char *end, size_t n)
{
    ASSERT_PTR(str, high, return NULL);
    ASSERT_PTR(end, high, return NULL);

    while (str < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            if (n-- == 0)
                return str;

        str = ui_text_next_symbol(str);
    }

    ERR_NEW(high, "Invalid Index - Not enough text", "");
    return NULL;
}

int32_t ui_text_decode_utf8(char *str, char *end)
{
    text_symbol_type typ;
    int32_t          rtn;
    size_t           width;

    ASSERT_PTR(str, high, return -1);
    ASSERT_PTR(end, high, return -1);

    typ = ui_text_symbol(*str);

    width = ui_text_symbol_width(typ);

    ASSERT(str + width < end,           high, return -1);
    ASSERT(ui_text_symbol_is_char(typ), high, return -1);

    if (typ == ascii)
        return (int32_t)*str;

    rtn = (0x7f >> (int)typ) & (*str);

    while (width--)
    {
        rtn <<= 6;
        rtn  |= *(++str) & 0x3f;
    }

    return rtn;
}
