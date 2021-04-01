#include "class_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String String_new(const char* string) {
    size_t len = strlen(string);
    // Allocate twice the required length
    char* dest = (char*)malloc(sizeof(char) * len * 2);
    // Copy and fill in with zeros.
    (void)strncpy(dest, string, len * 2);
    String string_obj = {.str = dest, .len = len * 2};
    return string_obj;
}

void String_renew(String* string_obj_p, const char* new_str) {
    size_t new_len = strlen(new_str);
    if (new_len >= string_obj_p->len) {
        string_obj_p->str = (char*)reallocf(string_obj_p->str, new_len * 2);
    }
    strncpy(string_obj_p->str, new_str, new_len * 2);
}

void String_destroy(String* string_obj_p) {
    free(string_obj_p->str);
    string_obj_p->str = NULL;
    string_obj_p = NULL;
}

Result_void String_print(const String* string_obj_p) {
    STRING_SANITY_BEGIN(string_obj_p)
    for (size_t i = 0; i < string_obj_p->len; i++) { printf("%c", string_obj_p->str[i]); }
    STRING_SANITY_END
}

Result_void String_println(const String* string_obj_p) {
    Result_void result = String_print(string_obj_p);
    if (result.error_code != 0) { printf("\n"); }
    return result;
}


Result_void String_display(const String* string_obj_p) {
    STRING_SANITY_BEGIN(string_obj_p)
    printf("String: `");
    size_t null_counter = 0;
    for (size_t i = 0; i < string_obj_p->len; i++) {
        if (string_obj_p->str[i] == '\0') {
            null_counter++;
            printf("<null>");
        } else {
            printf("%c", string_obj_p->str[i]);
        }
    }
    printf("`\n");
    printf("Null chars: `%lu`\n", null_counter);
    printf("Allocated len: `%lu`\n", string_obj_p->len);
    printf("Displayed version: `");
    String_print(string_obj_p);
    printf("`\n");
    STRING_SANITY_END
}
