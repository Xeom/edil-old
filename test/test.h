#include <stdio.h>
#include <time.h>
#include "head.h"

int test_count;

#define COL_D(num) "\x1b[0;" #num "m"
#define COL_B(num) "\x1b[1;" #num "m"
#define COL_END    "\x1b[0m"

#define BRACKET(col, symbol) \
    COL_B(col) "[" COL_D(col) #symbol COL_B(col) "] " COL_END

#define TEST(expression, rtntype)                        \
    {                                                    \
        puts("LINE: " STRIFY(__LINE__));                 \
        printf(#expression                               \
               " -> %" #rtntype "\n\n", expression);     \
    }

#define RUNTEST(funct)                                                  \
    {                                                                   \
        double  dt;                                                     \
        clock_t start, end;                                             \
        PINFO("Testing " #funct "...");                                 \
        start = clock();                                                \
        test_ ## funct ();                                              \
        end   = clock();                                                \
        dt    = ((double)end - (double)start) /(double)CLOCKS_PER_SEC;  \
        PSUCC("Ran tests for " #funct " in %.4fs", dt);                 \
    }

#define PINFO(...)                                          \
    {                                                       \
        fprintf(stderr, BRACKET(35, i) __VA_ARGS__);        \
        fputs("\n", stderr);                                \
    }

#define PWARN(...)                                          \
    {                                                       \
        fprintf(stderr, BRACKET(33, w) __VA_ARGS__);        \
        fputs("\n", stderr);                                \
    }

#define PSUCC(...)                                          \
    {                                                       \
        fprintf(stderr, BRACKET(32, .) __VA_ARGS__);        \
        fputs("\n", stderr);                                \
    }

#define PERR(...)                                           \
    {                                                       \
        fprintf(stderr, BRACKET(31, !) __VA_ARGS__);        \
        fputs("\n", stderr);                                \
    }
