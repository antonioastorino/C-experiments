#ifndef MY_STRING_H
#define MY_STRING_H
#include "result.h"
#include <sys/types.h>
#define STRING_SANITY_BEGIN(string_obj_p)                                                          \
    if (string_obj_p == NULL) {                                                                    \
        return Err(NULL, "Unitialized String object.", -1);                                        \
    } else if (string_obj_p->str == NULL) {                                                        \
        return Err(NULL, "Unitialized string in String object.", -1);                              \
    } else {
#define STRING_SANITY_END                                                                          \
    return Ok(NULL);                                                                               \
    }

typedef struct {
    // Array of chars whose allocated length >= `len`.
    char* str;
    // Length the user should be aware of.
    size_t len;
} String;

String String_new(const char*);
// Replace the internal string and reallocate its memory if necessary.
void String_renew(String *, const char*);
void String_destroy(String*);
Result_void String_print(const String*);
Result_void String_println(const String*);
Result_void String_display(const String*);

#endif
