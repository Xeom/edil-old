#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "head.h"

int test_count;

#if defined(TEST_NOPRINT)
# define TEST(expression, rtntype) \
    {expression}
#else
# define TEST(expression, rtntype)                       \
    {                                                    \
        puts("LINE: " STRIFY(__LINE__));                 \
        printf(#expression                               \
               " -> %" #rtntype "\n\n", expression);     \
    }
#endif

#define RUNTEST(funct)                                                  \
    {                                                                   \
        double  dt;                                                     \
        clock_t start, end;                                             \
        PLOG(info, "Testing " #funct "...");                            \
        start = clock();                                                \
        test_ ## funct ();                                              \
        end   = clock();                                                \
        dt    = ((double)end - (double)start) /(double)CLOCKS_PER_SEC;  \
        PLOG(succ, "Ran tests for " #funct " in %.4fs", dt);            \
    }

#define PLOG(type, ...)                                         \
    {                                                           \
        char cmd[2000];                                         \
        sprintf(cmd, STRIFY(ROOTDIR) "/misc/plog.sh " #type " " __VA_ARGS__); \
        system(cmd);                                            \
    }
