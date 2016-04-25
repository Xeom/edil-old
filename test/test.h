#include <stdio.h>
#include <time.h>

int test_count;

#define COL_D(num) "\x1b[0;" #num "m"
#define COL_B(num) "\x1b[1;" #num "m"
#define COL_END    "\x1b[0m"

#define BRACKET(col, symbol) \
    COL_B(col) "[" COL_D(col) #symbol COL_B(col) "] " COL_END

#define TEST(expression, rtntype)               \
    printf(#expression                          \
           " -> %" #rtntype "\n", expression);

#define RUNTEST(funct)                                                  \
    {                                                                   \
        double  dt;                                                     \
        clock_t start, end;                                             \
        fputs(BRACKET(35, i) "Testing " #funct "... \n", stderr);       \
        start = clock();                                                \
        test_ ## funct ();                                              \
        end   = clock();                                                \
        dt    = ((double)end - (double)start) / (double)CLOCKS_PER_SEC; \
        fprintf(stderr,                                                 \
                BRACKET(32, .) "Ran tests for " #funct " in %fs\n", dt);  \
    }
