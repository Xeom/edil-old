#define filename_dup(str) (strcpy(alloca(strlen(str)), str))

inline char *filename_pop(char *fn)
{
    char *rtn;
    rtn = strrchr(fn, "/");

    if (rtn)
        *(rtn++) = '\0';

    return rtn;
}
