#include "class_string.h"
#include "common.h"
#include "mem.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_FACTOR 1.5

bool String_is_null(const String* string_obj_p)
{
    if ((string_obj_p == NULL) || (string_obj_p->str == NULL))
        return true;
    return false;
}

Result_String_p String_new(const char* format, ...)
{
    va_list args;
    char* tmp_str_p = NULL;
    String* out_string_obj_p;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (VASPRINTF(&tmp_str_p, format, args) == -1)
    {
        LOG(ERROR, "Something went wrong with vasprintf - errno: %d", errno)
        return Err(out_string_obj_p, "Failed to create string.", errno);
    }
    size_t actual_size = strlen(tmp_str_p);
    // Allocate twice the required length
    size_t allocated_size = (size_t)(actual_size * SIZE_FACTOR);
    // printf("Allocated size: %zu\n", allocated_size);
    tmp_str_p = (char*)REALLOCF(tmp_str_p, sizeof(char) * allocated_size);
    LOG(TRACE, "Created string.")
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    out_string_obj_p         = (String*)MALLOC(sizeof(String));
    out_string_obj_p->str    = tmp_str_p;
    out_string_obj_p->length = actual_size;
    out_string_obj_p->size   = allocated_size;
    return Ok(out_string_obj_p);
}

Result_String_p String_clone(const String* origin) { return String_new(origin->str); }

Result_void_p String_renew(String* string_obj_p, const char* new_format, ...)
{
    if (String_is_null(string_obj_p))
    {
        return Err(NULL, "The provided string points to NULL.", -1);
    }
    va_list args;
    char* tmp_str_p = NULL;
    va_start(args, new_format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (VASPRINTF(&tmp_str_p, new_format, args) == -1)
    {
        return Err(NULL, "Failed to parse format.", errno);
    }
    size_t new_len = strlen(tmp_str_p);

    // Update string.
    if (new_len >= string_obj_p->size)
    {
        // Increase the allocated size.
        string_obj_p->size = (size_t)(new_len * SIZE_FACTOR);
        string_obj_p->str  = (char*)REALLOCF(string_obj_p->str, sizeof(char) * string_obj_p->size);
    }
    string_obj_p->length = new_len;
    // Copy an extra byte for the NULL characther.
    strncpy(string_obj_p->str, tmp_str_p, new_len + 1);
    FREE(tmp_str_p);
    return Ok(NULL);
}

void String_destroy(String* string_obj_p)
{
    FREE(string_obj_p->str);
    string_obj_p->str    = NULL;
    string_obj_p->length = -1;
    string_obj_p->size   = -1;

    FREE(string_obj_p);
    string_obj_p = NULL;
}

Result_void_p String_print(const String* string_obj_p)
{
    if (String_is_null(string_obj_p))
    {
        return Err(NULL, "Uninitialized string.", -1);
    }
    for (size_t i = 0; i < string_obj_p->length; i++)
    {
        printf("%c", string_obj_p->str[i]);
    }
    return Ok(NULL);
}

Result_void_p String_println(const String* string_obj_p)
{
    Result_void_p result = String_print(string_obj_p);
    RET_ON_ERR(result);
    printf("\n");
    return result;
}

Result_void_p String_display(const String* string_obj_p)
{
    if (String_is_null(string_obj_p))
    {
        return Err(NULL, "Uninitialized string.", -1);
    }
    printf("String: `");
    for (size_t i = 0; i < string_obj_p->size; i++)
    {
        if (string_obj_p->str[i] == '\0')
        {
            printf("<>");
        }
        else
        {
            printf("%c", string_obj_p->str[i]);
        }
    }
    printf("`\n");
    printf("Allocated length: `%lu`\n", string_obj_p->length);
    printf("Displayed version: `");
    String_print(string_obj_p);
    printf("`\n");
    return Ok(NULL);
}

// TODO: Result_bool - check if is null
bool String_starts_with(String* string_p, const char* prefix)
{
    if (strstr(string_p->str, prefix) == string_p->str)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Result_void_p String_replace_char(String* string_obj_haystack_p, const char needle,
                                  const char replace)
{
    if (String_is_null(string_obj_haystack_p))
    {
        return Err(NULL, "Uninitialized string.", -1);
    }
    char tmp_str[string_obj_haystack_p->length + 1];
    size_t i = 0, j = 0;
    while (i < string_obj_haystack_p->length)
    {
        if (string_obj_haystack_p->str[i] == needle)
        {
            if (replace != '\0')
            {
                // Replace the current char with that provided.
                tmp_str[j++] = replace;
            }
        }
        else
        {
            tmp_str[j++] = string_obj_haystack_p->str[i];
        }
        i++;
    }
    // Terminate.
    tmp_str[j] = '\0';
    // Update the string length in case some chars were removed.
    string_obj_haystack_p->length = j;
    strncpy(string_obj_haystack_p->str, tmp_str, j + 1);
    return Ok(NULL);
}

#if TEST == 1
void test_class_string()
{
    Result_void_p res_void;
    String* test_string_p;
    PRINT_BANNER("String class")

    PRINT_TEST_TITLE("New from string")
    const char* str = "Hello everybody";
    test_string_p   = unwrap(String_new(str));
    ASSERT_EQ(test_string_p->length, strlen(str), "Length correct.")
    ASSERT_EQ(test_string_p->size, (size_t)(strlen(str) * 1.5), "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap(String_display(test_string_p)), NULL, "Display functions work.")
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("New from formatter");
    const char* format1   = "Old string content.";
    test_string_p         = unwrap(String_new(format1));
    size_t initial_length = strlen(test_string_p->str);
    size_t initial_size   = (size_t)(initial_length * 1.5);
    ASSERT_EQ(test_string_p->length, initial_length, "Length correct.")
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("Renew without resizing");
    unwrap(String_renew(test_string_p, "New string content -longer."));
    ASSERT_EQ(test_string_p->length, strlen(test_string_p->str), "Length correct.")
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("Renew with resizing");
    unwrap(String_renew(test_string_p, "The new string content is way too long."));
    ASSERT_EQ(test_string_p->length, strlen(test_string_p->str), "Length correct.")
    ASSERT_EQ(test_string_p->size, (size_t)(test_string_p->length * 1.5), "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(test_string_p, "The new"), true, "starts_with() works when true")
    ASSERT_EQ(String_starts_with(test_string_p, "new"), false, "starts_with() works when false")
    ASSERT_EQ(String_starts_with(test_string_p, ""), true,
              "starts_with() works when needle is empty")
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("clone() function")
    String* test_origin = unwrap(String_new("Original"));
    String* test_clone  = unwrap(String_clone(test_origin));
    size_t length       = test_origin->length;
    size_t size         = test_origin->size;
    ASSERT_EQ(strcmp((const char*)test_origin->str, (const char*)test_clone->str), 0,
              "Strings match.")
    String_destroy(test_origin); // The clone should still be alive.
    ASSERT_EQ(length, test_clone->length, "Size copied")
    ASSERT_EQ(size, test_clone->size, "Size copied")
    String_destroy(test_clone);

    PRINT_TEST_TITLE("replace_char() function - replace with nothing.");
    test_string_p = unwrap(String_new("Some text to be modified."));
    String_println(test_string_p);
    unwrap(String_replace_char(test_string_p, ' ', '\0'));
    ASSERT_EQ(strcmp(test_string_p->str, "Sometexttobemodified."), 0, "Needles removed correctly.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, 't', '_'));
    ASSERT_EQ(strcmp(test_string_p->str, "Some_ex__obemodified."), 0, "Needles replaced correctly.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, '&', '^'));
    ASSERT_EQ(strcmp(test_string_p->str, "Some_ex__obemodified."), 0,
              "String unchanged - needle not found.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, 'S', '+'));
    unwrap(String_replace_char(test_string_p, 'o', '+'));
    unwrap(String_replace_char(test_string_p, 'm', '+'));
    unwrap(String_replace_char(test_string_p, '_', '+'));
    unwrap(String_replace_char(test_string_p, 'e', '+'));
    unwrap(String_replace_char(test_string_p, 'x', '+'));
    unwrap(String_replace_char(test_string_p, 'b', '+'));
    unwrap(String_replace_char(test_string_p, 'd', '+'));
    unwrap(String_replace_char(test_string_p, 'i', '+'));
    unwrap(String_replace_char(test_string_p, 'f', '+'));
    unwrap(String_replace_char(test_string_p, '.', '+'));
    ASSERT_EQ(strcmp(test_string_p->str, "+++++++++++++++++++++"), 0,
              "Replaced all chars in string with '+'.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, '+', '\0'));
    ASSERT_EQ(strcmp(test_string_p->str, ""), 0, "Ths string is correctly emptied.")
    ASSERT_EQ(test_string_p->length, 0, "Ths length is correctly set to 0.")
    String_println(test_string_p);
    String_destroy(test_string_p);
}
#endif