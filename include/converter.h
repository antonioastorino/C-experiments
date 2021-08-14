#ifndef CONVERTER_H
#define CONVERTER_H

#include "common.h"
#include "result.h"

#ifdef __cplusplus
extern "C"
{
#endif

    Result_int str_to_int(const char* str_p);
    Result_uint str_to_uint(const char* str_p);
    Result_float str_to_float(const char* str_p);
    float rounder(float to_be_rounded, float step, size_t num_of_decimals);

#if TEST == 1
    void test_converter(void);
#endif
#ifdef __cplusplus
};
#endif
#endif