#ifndef MY_STRING_H
#define MY_STRING_H
#include "result.h"
#include <sys/types.h>

typedef struct class_string {
    // Array of chars whose allocated length >= `len`.
    char* str;
    // Length that would be returned by `strlen(str)`.
    size_t length;
    // Allocated memory in number of chars.
    size_t size;
} String;

String String_new(const char*, ...);
// Replace the internal string and reallocate its memory if necessary.
void String_renew(String*, const char*, ...);
void String_destroy(String*);
int String_print(const String*);
int String_println(const String*);
int String_display(const String*);

bool String_starts_with(String*, const char*);

#endif
