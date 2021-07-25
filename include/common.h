#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>

#define TEST 1
#define LOG_LEVEL 5
#define MEM_ANALYSIS 1

// ----------- Error codes ---------- //
typedef enum
{
    ERR_ALL_GOOD = 0,
    ERR_FATAL,
    ERR_NULL,
    ERR_GENERIC,
    ERR_NOT_FOUND,
    ERR_CREATE_STRING,
    ERR_EMPTY_STRING,
    ERR_JSON_INVALID,
    ERR_TYPE_MISMATCH,
    ERR_PARSE_STRING_TO_INT,
    ERR_PARSE_STRING_TO_FLOAT,
    ERR_FOLDER_EXISTS,
    ERR_FOLDER_NON_EMPTY,
    ERR_PERMISSION_DENIED,
    ERR_INVALID_INPUT,
    ERR_NETWORK_FAILURE,
} ErrorCode;
// ---------------------------------- //

#if TEST == 1
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

#define PRINT_BANNER()                                                                             \
    printf("\n");                                                                                  \
    for (size_t i = 0; i < strlen(__FUNCTION__) + 12; i++)                                         \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n-- TEST: %s --\n", __FUNCTION__);                                                    \
    for (size_t i = 0; i < strlen(__FUNCTION__) + 12; i++)                                         \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n");                                                                                  \
    size_t counter = 0;

#define PRINT_TEST_TITLE(title) printf("\n------- T:%lu < %s > -------\n", ++counter, title);
#endif
#endif