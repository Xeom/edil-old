typedef struct line_s line;

struct line_s
{
    chunk *c;
    size_t len;
    char   text[];
};

static line *buffer_line_realloc(line *l, size_t textlen)
{
    size_t newsize;

    newsize = sizeof(line) + textlen + 1;

    return realloc(l, newsize);
}

line *buffer_line_init(void);
{
    line *rtn;

    rtn = buffer_line_realloc(NULL, 0);
    rtn->len = 1;
    rtn->text[0] = '\0';

    return rtn;
}

void buffer_line_free(line *l);
{
    free(l);
}

line *buffer_line_set_text(line *l, const char *text);
{
    size_t newlen;
    line *rtn;

    newlen = strlen(text);
    rtn    = buffer_line_realloc(l, newlen);

    strcpy(rtn->text, text);
}

const char *buffer_line_get_text(line *l);
{
    return (char *)l->text;
}

int buffer_line_set_chunk(line *l, chunk *c)
{
    l->c = c;

    return 0;
}

char *buffer_line_copy_text(line *l);
{
    char *rtn;

    rtn = malloc(l->len + 1);
    strcpy(rtn, l->text);

    return rtn;
}
