#include "result.h"
#include <stdio.h>
#include <stdlib.h>

// Macro definition to create customized result types.
#define RESULT_TYPE_c(suffix, ret_type)                                                            \
                                                                                                   \
    Result_##suffix Ok_##suffix(ret_type value)                                                    \
    {                                                                                              \
        ReturnValue ok;                                                                            \
        Error err           = {.message = "", .code = 0};                                          \
        ok.ret_##suffix     = value;                                                               \
        Result_##suffix res = {                                                                    \
            .is_err = false,                                                                       \
            .ok     = ok,                                                                          \
            .err    = err,                                                                         \
        };                                                                                         \
        return res;                                                                                \
    }                                                                                              \
    Result_##suffix Err_##suffix(const char* message, int code)                                    \
    {                                                                                              \
        ReturnValue ok;                                                                            \
        Error err                 = {.message = message, .code = code};                            \
        ok.ret_void_p             = NULL;                                                          \
        Result_##suffix ret_value = {.is_err = true, .err = err, .ok = ok};                        \
        return ret_value;                                                                          \
    }                                                                                              \
                                                                                                   \
    ret_type unwrap_##suffix(Result_##suffix ret_value)                                            \
    {                                                                                              \
        if (ret_value.is_err)                                                                      \
        {                                                                                          \
            fprintf(stderr, "Panic! Trying to unwrap an error.");                                  \
            exit(1);                                                                               \
        }                                                                                          \
        return ret_value.ok.ret_##suffix;                                                          \
    }                                                                                              \
                                                                                                   \
    Error unwrap_err_##suffix(Result_##suffix value)                                               \
    {                                                                                              \
        if (!value.is_err)                                                                         \
        {                                                                                          \
            fprintf(stderr, "Panic! Trying to unwrap the error from a valid result");              \
            exit(1);                                                                               \
        }                                                                                          \
        return value.err;                                                                          \
    }

RESULT_TYPE_c(int, int);
RESULT_TYPE_c(float, float);
RESULT_TYPE_c(char_p, const char*);
RESULT_TYPE_c(String_p, String*);
RESULT_TYPE_c(JsonObj_p, JsonObj*);
RESULT_TYPE_c(void_p, void*);
