#ifndef CONVERTER_H
#define CONVERTER_H

#include "result.h"

Result_int str_to_int(char* str_p);

#if TEST == 1
void test_converter(void);
#endif

#endif