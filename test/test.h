#include <stdio.h>

int test_count;


#define TEST(expression, rtntype)               \
    printf(#expression                          \
           " -> %" #rtntype "\n", expression);
