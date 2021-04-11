#ifndef CONVERTER_H
#define CONVERTER_H

#include "common.h"
#include "result.h"

Result_int str_to_int(char* str_p);
Result_float str_to_float(char* str_p);

#if TEST == 1
void test_converter(void);
#endif

#endif