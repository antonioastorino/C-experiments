#ifndef RESULT_H
#define RESULT_H

typedef struct class_string String;

#include <stdbool.h>
typedef enum {
    type_int,
    type_float,
    type_char_p,
    type_void_p,
    type_String_p,
    type_error,
} ResultType;

typedef union {
    int ret_int;
    float ret_float;
    const char* ret_char_p;
    String* ret_String_p;
    void* ret_void_p;
} ReturnValue;

typedef struct {
    const char* message;
    int code;
} Error;

typedef struct {
    ResultType type;
    Error err;
    ReturnValue ok;
} Result;

#define RESULT_TYPE_h(suffix, ret_type)                                                            \
    Result Ok_##suffix(ret_type);                                                                  \
    ret_type unwrap_##suffix(Result);

Result Err(const char* message, int code);
Error unwrap_err(Result);

RESULT_TYPE_h(int, int);
RESULT_TYPE_h(float, float);
RESULT_TYPE_h(char_p, const char*);
RESULT_TYPE_h(void_p, void*);
RESULT_TYPE_h(String_p, String*);

#define Ok(ret_value)                                                                              \
    _Generic(ret_value,                                                                             \
        int : Ok_int,                                                                               \
        float : Ok_float,                                                                           \
        const char* : Ok_char_p,                                                                    \
        String *   : Ok_String_p,                                                                   \
        void *    : Ok_void_p)(ret_value);

/// Return an error if `result` is an error.
#define RET_ON_ERR(result)                                                                         \
    if (result.type == type_error) { return result; }

#endif