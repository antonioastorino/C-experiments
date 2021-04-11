#ifndef MEM_H
#define MEM_H
#include <ctype.h>

void* custom_malloc(size_t, const char*, const int);
void* custom_reallocf(void*, size_t, const char*, const int);
void custom_free(void*, const char*, const int);

#define MALLOC(size) custom_malloc(size, __FILE__, __LINE__)
#define REALLOCF(ptr, size) custom_reallocf(ptr, size, __FILE__, __LINE__);

#define FREE(ptr) custom_free(ptr, __FILE__, __LINE__)

#endif