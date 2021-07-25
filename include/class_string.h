#ifndef MY_STRING_H
#define MY_STRING_H
#include "common.h"
#include "result.h"
#include <stdbool.h>
#include <sys/types.h>

typedef struct class_string
{
    // Length that would be returned by `strlen(str)`.
    size_t length;
    // Allocated memory in number of chars.
    size_t size;
    // Array of chars whose allocated length >= `len`.
    char* str;
} String;

/************************************* (De)Constructors ***************************************/
String* String_new(const char*, ...);
String* String_clone(const String*);
void String_destroy(String*);

/****************************************** Printers ******************************************/
Result_void_p String_print(const String*);
Result_void_p String_println(const String*);

/***************************************** Checkers *******************************************/
bool String_is_null(const String*);
bool String_starts_with(const String*, const char*);
Result_String_p String_between_patterns_in_string_p(String*, const char*, const char*);
Result_String_p String_between_patterns_in_char_p(const char*, const char*, const char*);

/**************************************** Modifiers *******************************************/
Result_void_p String_replace_char(String*, const char, const char);
Result_void_p String_replace_pattern(String*, const char*, const char*);

// clang-format off
#define String_between_patterns(in_value, prefix, suffix) \
    _Generic((in_value), \
     String*     : String_between_patterns_in_string_p, \
     const char* : String_between_patterns_in_char_p \
     )(in_value, prefix, suffix)
// clang-format on

#if TEST == 1
void test_class_string(void);
#endif

#endif
