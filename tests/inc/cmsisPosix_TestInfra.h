#include <stdio.h>
#include <stdlib.h>

#define CP_COLOR_RED     "\033[31m"
#define CP_COLOR_GREEN   "\033[32m"
#define CP_COLOR_RESET   "\033[0m"

#define CP_ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, CP_COLOR_RED "ASSERT TRUE FAILED" CP_COLOR_RESET ": %s\n  in %s:%d\n", \
                    #expr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CP_ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            fprintf(stderr, CP_COLOR_RED "ASSERT EQ FAILED" CP_COLOR_RESET ": %s == %s\n  Got %lld != %lld\n  in %s:%d\n", \
                    #a, #b, (long long)(a), (long long)(b), __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CP_ASSERT_NE(a, b) \
    do { \
        if ((a) == (b)) { \
            fprintf(stderr, CP_COLOR_RED "ASSERT NE FAILED" CP_COLOR_RESET ": %s != %s\n  Got %lld == %lld\n  in %s:%d\n", \
                    #a, #b, (long long)(a), (long long)(b), __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

extern void test_start(void);

int main(void)
{
    test_start();

    return 0;
}
