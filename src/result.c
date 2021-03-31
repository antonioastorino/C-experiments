#include "result.h"

// Macro definition to create customized result types.
#define RESULT_TYPE_c(suffix, type)                                            \
                                                                               \
  Result_##suffix Ok_##suffix(type ok_result) {                                \
    Result_##suffix res = {                                                    \
        .is_err = false,                                                       \
        .ok = ok_result,                                                       \
        .err = "",                                                             \
    };                                                                         \
    return res;                                                                \
  }                                                                            \
                                                                               \
  Result_##suffix Err_##suffix(char *message) {                                \
    Result_##suffix res = {                                                    \
        .is_err = true,                                                        \
        .err = message,                                                        \
    };                                                                         \
    return res;                                                                \
  }

RESULT_TYPE_c(int, int);
RESULT_TYPE_c(float, float);
RESULT_TYPE_c(char_p, char *);
RESULT_TYPE_c(void, void *);
