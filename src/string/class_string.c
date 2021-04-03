#include "class_string.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


String String_new(const char* format, ...) {
    va_list args;
    char* dest;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (vasprintf(&dest, format, args) == -1) {
        printf("Something went wrong.");
        exit(1);
    }
    size_t actual_size = strlen(dest);
    // printf("Calculated size: %zu\n", actual_size);
    // Allocate twice the required length
    size_t allocated_size = actual_size * 2;
    // printf("Allocated size: %zu\n", allocated_size);
    dest = (char*)reallocf(dest, sizeof(char) * allocated_size);
    // printf("Created string: %s\n", dest);
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    String string_obj = {.str = dest, .length = actual_size, .size = allocated_size};

    // printf("Saved string: %s\n", string_obj.str);
    // printf("Saved string size: %lu\n", string_obj.size);

    return string_obj;
}

void String_renew(String* string_obj_p, const char* new_str) {
    size_t new_len = strlen(new_str);
    if (new_len >= string_obj_p->length) {
        string_obj_p->str = (char*)reallocf(string_obj_p->str, new_len * 2);
    }
    strncpy(string_obj_p->str, new_str, new_len * 2);
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
    if (string_obj_p == NULL) {
        return -1;
    }
    printf("String: `");
    size_t null_counter = 0;
    for (size_t i = 0; i < string_obj_p->size; i++) {
        if (string_obj_p->str[i] == '\0') {
            null_counter++;
            printf("<null>");
        } else {
            printf("%c", string_obj_p->str[i]);
        }
    }
    printf("`\n");
    printf("Null chars: `%lu`\n", null_counter);
    printf("Allocated length: `%lu`\n", string_obj_p->length);
    printf("Displayed version: `");
    String_print(string_obj_p);
    printf("`\n");
    return 0;
}
