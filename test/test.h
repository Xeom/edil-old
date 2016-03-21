int test_count;

#define TEST_ASSERT(condition, msg, ...)                                \
    {                                                                   \
        ++test_count;                                                   \
        if (!condition)                                                 \
        {                                                               \
            puts("HO");                                                 \
            char *_assert_msg;                                          \
            _assert_msg = malloc(1000);                                 \
            snprintf(_assert_msg, 1000, "[!] Test failed: " msg "\n",   \
                     __VA_ARGS__);                                      \
            fputs(_assert_msg, stderr);                                 \
            free(_assert_msg);                                          \
            return -1;                                                  \
        }                                                               \
    }
