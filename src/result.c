#include "result.h"
#include <stdio.h>
#include <stdlib.h>

// Macro definition to create customized result types.
#define RESULT_TYPE_c(suffix, ret_type)                                                            \
                                                                                                   \
    Result Ok_##suffix(ret_type value)                                                             \
    {                                                                                              \
        ReturnValue ok;                                                                            \
        ResultType type = type_##suffix;                                                           \
        Error err       = {.message = "", .code = 0};                                              \
        ok.ret_##suffix = value;                                                                   \
        Result res      = {                                                                        \
            type = type,                                                                      \
            .ok  = ok,                                                                        \
            .err = err,                                                                       \
        };                                                                                    \
        return res;                                                                                \
    }                                                                                              \
    ret_type unwrap_##suffix(Result ret_value)                                                     \
    {                                                                                              \
        if (ret_value.type != type_##suffix)                                                       \
        {                                                                                          \
            fprintf(stderr,                                                                        \
                    "Panic! Trying to unwrap a result of type " #suffix " with wrong unwrapper");  \
            exit(1);                                                                               \
        }                                                                                          \
        return ret_value.ok.ret_##suffix;                                                          \
    }

Result Err(const char* message, int code)
{
    ReturnValue ok;
    Error err        = {.message = message, .code = code};
    ResultType type  = type_error;
    ok.ret_void_p    = NULL;
    Result ret_value = {.err = err, .ok = ok, .type = type};
    return ret_value;
}

Error unwrap_err(Result value)
{
    if (value.type != type_error)
    {
        fprintf(stderr, "Panic! Trying to unwrap the error from a valid result");
        exit(1);
    }
    return value.err;
}

RESULT_TYPE_c(int, int);
RESULT_TYPE_c(float, float);
RESULT_TYPE_c(char_p, const char*);
RESULT_TYPE_c(void_p, void*);
RESULT_TYPE_c(String_p, String*);
