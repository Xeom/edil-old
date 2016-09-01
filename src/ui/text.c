typedef enum {
    ascii = 0x01,
    uni_2 = 0x02,
    uni_3 = 0x03,
    uni_4 = 0x04,
    face  = 0x81,
    uni_x = 0x82
} text_symbol_type;

text_symbol_type ui_text_symbol(uchar c)
{
    if (c == '\n')
        return face;

    if (0x80 & c == 0x00)
        return ascii;

    if (0xc0 & c == 0x80)
        return uni_x;

    text_symbol_type rtn = ascii;

    while (0x40 & c)
    {
        c  <<= 1;
        rtn += 1;
    }

    return rtn;
}

size_t ui_text_symbol_width(text_symbol_type sym)
{
    switch (sym)
    {
    case face:
        return face_serialized_len;
    case uni_x:
        return 1;
    default:
        return (int)sym;
    }
}

static inline ui_text_next_symbol(char *str)
{
    return str + ui_text_symbol_width(ui_text_symbol(*str));
}

ui_text_symbol_is_char(text_symbol_type sym)
{
    return sym <= uni_4;
}

static char *ui_text_next_char(char *str, char *end)
{
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

static char *ui_text_next_face(char *str, char *end)
{
    while (str < end &&
           (str = ui_text_next_symbol(str)) < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (typ == face)
            return str;
    }

    return NULL;
}

static size_t ui_text_len(char *str, char *end)
{
    size_t rtn;

    rtn = 0;

    while (str < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            rtn++;

        str = ui_text_next_symbol(str)
    }

    return rtn;
}

static char *ui_text_get_char(char *str, char *end, size_t n)
{
    while (str < end)
    {
        text_symbol_type typ;

        typ = ui_text_symbol(*str);

        if (ui_text_symbol_is_char(typ))
            if (n-- == 0)
                return str;

        str = ui_text_next_symbol(str);
    }

    return rtn;
}

