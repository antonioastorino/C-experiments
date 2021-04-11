#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#define MEM_ANALYSIS 1

void* custom_malloc(size_t size, const char* file, const int line)
{
    void* ptr = malloc(size);
#if MEM_ANALYSIS == 1
    printf("PTR: %p (%lu) - %s:%d\n", ptr, size, file, line);
#endif
    return ptr;
}

void* custom_reallocf(void* ptr, size_t size, const char* file, const int line)
{
    void* new_ptr = reallocf(ptr, size);
    printf("PTR: %p (%lu) - %s:%d\n", new_ptr, size, file, line);
    return new_ptr;
}

void custom_free(void* ptr, const char* file, const int line)
{
    free(ptr);
    printf("PTR: %p - %s:%d\n", ptr, file, line);
}