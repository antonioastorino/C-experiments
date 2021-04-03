#include "class_string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String String_new(const char* format, ...) {
    va_list args;
    char* tmp_str_p;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (vasprintf(&tmp_str_p, format, args) == -1) {
        printf("Something went wrong.");
        exit(1);
    }
    size_t actual_size = strlen(tmp_str_p);
    // Allocate twice the required length
    size_t allocated_size = actual_size * 2;
    // printf("Allocated size: %zu\n", allocated_size);
    tmp_str_p = (char*)reallocf(tmp_str_p, sizeof(char) * allocated_size);
    // printf("Created string: %s\n", tmp_str_p);
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    String string_obj = {.str = tmp_str_p, .length = actual_size, .size = allocated_size};

    return string_obj;
}

void String_renew(String* string_obj_p, const char* new_format, ...) {
    va_list args;
    char* tmp_str_p;
    va_start(args, new_format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (vasprintf(&tmp_str_p, new_format, args) == -1) {
        printf("Something went wrong.");
        exit(1);
    }
    size_t new_len = strlen(tmp_str_p);
    if (new_len >= string_obj_p->size) {
        string_obj_p->str  = (char*)reallocf(string_obj_p->str, sizeof(char) * new_len * 2);
        string_obj_p->size = new_len * 2;
    }
    // Copy an extra byte for the NULL characther.
    strncpy(string_obj_p->str, tmp_str_p, new_len + 1);
    free(tmp_str_p);
    string_obj_p->length = new_len;
}

void String_destroy(String* string_obj_p) {
    free(string_obj_p->str);
    string_obj_p->str = NULL;
}

int String_print(const String* string_obj_p) {
    if (string_obj_p == NULL) { return -1; }
    for (size_t i = 0; i < string_obj_p->length; i++) { printf("%c", string_obj_p->str[i]); }
    return 0;
}

int String_println(const String* string_obj_p) {
    if (String_print(string_obj_p) == -1) return -1;
    printf("\n");
    return 0;
}

int String_display(const String* string_obj_p) {
    if (string_obj_p == NULL) { return -1; }
    printf("String: `");
    for (size_t i = 0; i < string_obj_p->size; i++) {
        if (string_obj_p->str[i] == '\0') {
            printf("<null>");
        } else {
            printf("%c", string_obj_p->str[i]);
        }
    }
    printf("`\n");
    printf("Allocated length: `%lu`\n", string_obj_p->length);
    printf("Displayed version: `");
    String_print(string_obj_p);
    printf("`\n");
    return 0;
}
