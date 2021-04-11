#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>

#define TEST 1
#define LOG_LEVEL 1
#define MEM_ANALYSIS 1

#define ASSERT_EQ(value_1, value_2, message)                                                       \
    if (value_1 == value_2)                                                                        \
    {                                                                                              \
        printf("\n> \e[32mPASS\e[0m\t %s\n", message);                                             \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        printf("\n> \e[31mFAIL\e[0m\t %s\n", message);                                             \
        fprintf(stderr, "\n> Err - Test failed.\n%s:%d : left != right\n", __FILE__, __LINE__);    \
    }

#define PRINT_BANNER(title)                                                                        \
    printf("\n");                                                                                  \
    for (size_t i = 0; i < strlen(title) + 12; i++)                                                \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n-- TEST: %s --\n", title);                                                           \
    for (size_t i = 0; i < strlen(title) + 12; i++)                                                \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n");                                                                                  \
    size_t counter = 0;

#define PRINT_TEST_TITLE(title) printf("\n------- T:%lu < %s > -------\n", ++counter, title);

#endif