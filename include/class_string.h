#ifndef MY_STRING_H
#define MY_STRING_H
#include "result.h"
#include <stdbool.h>
#include <sys/types.h>

typedef struct class_string
{
    // Array of chars whose allocated length >= `len`.
    char* str;
    // Length that would be returned by `strlen(str)`.
    size_t length;
    // Allocated memory in number of chars.
    size_t size;
} String;

/**************************************** (De)Constructors ****************************************/
Result_String_p String_new(const char*, ...);
// Replace the internal string and reallocate its memory if necessary.
Result_void_p String_renew(String*, const char*, ...);

void String_destroy(String*);

/******************************************** Printers ********************************************/
Result_void_p String_print(const String*);
Result_void_p String_println(const String*);
Result_void_p String_display(const String*);

/******************************************** Checkers ********************************************/
bool String_starts_with(String*, const char*);

/******************************************* Modifiers ********************************************/

#endif
