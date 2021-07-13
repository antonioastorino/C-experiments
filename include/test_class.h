#ifndef TEST_CLASS_H
#define TEST_CLASS_H
#include "common.h"

typedef struct class
{
    int value;
    void (*fun)(void);
} Class;

Class create(int);

#if TEST == 1
void test_test_class();
#endif
#endif