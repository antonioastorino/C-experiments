#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#if MEM_ANALYSIS == 1

void* custom_malloc(size_t size, const char* file, const int line)
{
    void* ptr = malloc(size);
    printf("ALLOC_PTR: %p (%lu) - %s:%d\n", ptr, size, file, line);
    return ptr;
}

void* custom_reallocf(void* ptr, size_t size, const char* file, const int line)
{
    void* new_ptr = reallocf(ptr, size);
    printf("REALLOC_PTR: %p (%lu) - %s:%d\n", new_ptr, size, file, line);
    return new_ptr;
}

void custom_free(void* ptr, const char* file, const int line)
{
    free(ptr);
    printf("FREE_PTR: %p - %s:%d\n", ptr, file, line);
}
#endif