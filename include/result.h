#ifndef RESULT_H
#define RESULT_H
#include "common.h"

typedef struct class_string String;
typedef struct class_json_obj JsonObj;

#include <stdbool.h>

typedef union
{
    int ret_int;
    float ret_float;
    const char* ret_char_p;
    String* ret_String_p;
    JsonObj* ret_JsonObj_p;
    void* ret_void_p;
} ReturnValue;

typedef struct
{
    const char* message;
    int code;
} Error;

#define RESULT_TYPE_h(suffix, ret_type)                                                            \
    typedef struct                                                                                 \
    {                                                                                              \
        bool is_err;                                                                               \
        Error err;                                                                                 \
        ReturnValue ok;                                                                            \
    } Result_##suffix;                                                                             \
    Result_##suffix Ok_##suffix(ret_type);                                                         \
    Result_##suffix Err_##suffix(const char*, int);                                                \
    Error unwrap_err_##suffix(Result_##suffix);                                                    \
    ret_type unwrap_##suffix(Result_##suffix);

RESULT_TYPE_h(int, int);
RESULT_TYPE_h(float, float);
RESULT_TYPE_h(char_p, const char*);
RESULT_TYPE_h(String_p, String*);
RESULT_TYPE_h(JsonObj_p, JsonObj*);
RESULT_TYPE_h(void_p, void*);

#define Ok(ret_value)                                                                              \
    _Generic(ret_value,                                                                             \
        int : Ok_int,                                                                               \
        float : Ok_float,                                                                           \
        const char* : Ok_char_p,                                                                    \
        String *   : Ok_String_p,                                                                   \
        JsonObj *   : Ok_JsonObj_p,                                                                   \
        void *    : Ok_void_p)(ret_value);

#define Err(type, message, code)                                                                   \
    _Generic(type,                                                                             \
        int : Err_int,                                                                               \
        float : Err_float,                                                                           \
        const char* : Err_char_p,                                                                    \
        String *   : Err_String_p,                                                                   \
        JsonObj *   : Err_JsonObj_p,                                                                   \
        void *    : Err_void_p)(message, code);

// clang-format off
#define unwrap(ret_value) \
    _Generic((ret_value), \
     Result_int : unwrap_int, \
     Result_float : unwrap_float, \
     Result_char_p : unwrap_char_p, \
     Result_String_p : unwrap_String_p, \
     Result_JsonObj_p : unwrap_JsonObj_p, \
     Result_void_p : unwrap_void_p \
     )(ret_value)

#define unwrap_err(ret_value) \
    _Generic((ret_value),\
     Result_int : unwrap_err_int, \
     Result_float  : unwrap_err_float, \
     Result_char_p : unwrap_err_char_p, \
     Result_String_p : unwrap_err_String_p, \
     Result_JsonObj_p : unwrap_err_JsonObj_p, \
     Result_void_p : unwrap_err_void_p \
     )(ret_value)
// clang-format on

/// Return an error if `result` is an error.
#define RET_ON_ERR(result)                                                                         \
    if (result.err.code)                                                                           \
    {                                                                                              \
        return result;                                                                             \
    }

#endif