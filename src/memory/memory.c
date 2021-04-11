#include "memory.h"
#include "common.h"
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
    if (new_ptr != ptr)
    {
        // Realloc, when creating a new pointer, frees the old one.
        printf("REALLOC_PTR: %p (%lu) - %s:%d\n", new_ptr, size, file, line);
        printf("FREE_PTR: %p (%lu) - %s:%d\n", ptr, size, file, line);
        ptr = NULL;
    }
    return new_ptr;
}

int custom_vasprintf(char** ptr_p, const char* format, va_list args, const char* file,
                     const int line)
{
    int ret_val = vasprintf(ptr_p, format, args);
    printf("ALLOC_PTR: %p (%d) - %s:%d\n", *ptr_p, ret_val, file, line);
    return ret_val;
}

void custom_free(void* ptr, const char* file, const int line)
{
    free(ptr);
    printf("FREE_PTR: %p - %s:%d\n", ptr, file, line);
}

#endif