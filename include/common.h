#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define TEST 0
#define LOG_LEVEL 5
#define MEM_ANALYSIS 0
// ---------------------------------- //
#include <stdio.h>
#include <string.h>
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
        ERR_JSON_MISSING_ENTRY,
        ERR_TYPE_MISMATCH,
        ERR_PARSE_STRING_TO_INT,
        ERR_PARSE_STRING_TO_FLOAT,
        ERR_FOLDER_EXISTS,
        ERR_FOLDER_NON_EMPTY,
        ERR_PERMISSION_DENIED,
        ERR_INVALID_INPUT,
        ERR_PARSE_STRING_TO_LONG_INT,
    } ErrorCode;
    // ---------------------------------- //

#if TEST == 1

    void ASSERT_EQ_int(int, int, const char*, const char*, int);
    void ASSERT_EQ_size_t(size_t, size_t, const char*, const char*, int);
    void ASSERT_EQ_bool(bool v1, bool v2, const char*, const char*, int);
    void ASSERT_EQ_float(float, float, const char*, const char*, int);
    void ASSERT_EQ_double(double, double, const char*, const char*, int);
    void ASSERT_EQ_char_p(const char*, const char*, const char*, const char*, int);

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
    size_t test_counter_ = 0;

// clang-format off
#define ASSERT_EQ(value_1, value_2, message)    \
    _Generic((value_1),                         \
        int         : ASSERT_EQ_int,            \
        size_t      : ASSERT_EQ_size_t,         \
        bool        : ASSERT_EQ_bool,           \
        float       : ASSERT_EQ_float,          \
        double      : ASSERT_EQ_double,         \
        char*       : ASSERT_EQ_char_p,         \
        const char* : ASSERT_EQ_char_p          \
    )(value_1, value_2, message, __FILE__, __LINE__)
    // clang-format on

#define PRINT_TEST_TITLE(title) printf("\n------- T:%lu < %s > -------\n", ++test_counter_, title);
#endif
#ifdef __cplusplus
};
#endif
#endif