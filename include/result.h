#ifndef RESULT_H
#define RESULT_H

#include <stdbool.h>

#define RESULT_TYPE_h(suffix, type)                                                                \
    typedef struct {                                                                               \
        int error_code;                                                                            \
        type ok;                                                                                   \
        char* err;                                                                                 \
    } Result_##suffix;                                                                             \
                                                                                                   \
    Result_##suffix Ok_##suffix(type ok_result);                                                   \
                                                                                                   \
    Result_##suffix Err_##suffix(char* message, int error_code);

RESULT_TYPE_h(int, int);
RESULT_TYPE_h(float, float);
RESULT_TYPE_h(char_p, char*);
RESULT_TYPE_h(void, void*);

#define Err(unused, X, Y)                                                                          \
    _Generic(unused, int : Err_int, float : Err_float, char* : Err_char_p, void* : Err_void)(X, Y)
#define Ok(X) _Generic(X, int : Ok_int, float : Ok_float, char* : Ok_char_p, void* : Ok_void)(X)
#endif
