// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef ENABLE_TEST_LOG
#define TEST_LOG_INFO(fmt, ...) printf("[INFO]  (%s:%d): " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define TEST_LOG_INFO(fmt, ...)  ((void)0)
#endif

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

#define CP_ASSERT_STREQ(a, b) \
    do { \
        if (strcmp((a), (b)) != 0) { \
            fprintf(stderr, CP_COLOR_RED "ASSERT STREQ FAILED" CP_COLOR_RESET ": %s == %s\n  Got \"%s\" != \"%s\"\n  in %s:%d\n", \
                    #a, #b, (a), (b), __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CP_ASSERT_STRNE(a, b) \
    do { \
        if (strcmp((a), (b)) == 0) { \
            fprintf(stderr, CP_COLOR_RED "ASSERT STRNE FAILED" CP_COLOR_RESET ": %s != %s\n  Got \"%s\" == \"%s\"\n  in %s:%d\n", \
                    #a, #b, (a), (b), __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CP_ASSERT_UNREACHABLE() \
    do { \
        fprintf(stderr, CP_COLOR_RED "UNREACHABLE CODE REACHED" CP_COLOR_RESET "\n  in %s:%d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } while (0)

extern void test_start(void);

int main(void)
{
    time_t seed = time(NULL);
    printf("test seed - %ld\n", seed);
    srand(seed);    // Initialise random number generator

    test_start();

    return 0;
}
