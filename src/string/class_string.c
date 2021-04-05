#include "class_string.h"
#include "logger.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Result_String_p String_new(const char* format, ...)
{
    va_list args;
    char* tmp_str_p = NULL;
    String* out_p_string_obj;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (vasprintf(&tmp_str_p, format, args) == -1)
    {
        LOG(ERROR, "Something went wrong with vasprintf - errno: %d", errno)
        return Err(out_p_string_obj, "Failed to create string.", errno);
    }
    size_t actual_size = strlen(tmp_str_p);
    // Allocate twice the required length
    size_t allocated_size = actual_size * 2;
    // printf("Allocated size: %zu\n", allocated_size);
    tmp_str_p = (char*)reallocf(tmp_str_p, sizeof(char) * allocated_size);
    LOG(TRACE, "Created string: %s", tmp_str_p)
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    out_p_string_obj = (String*)malloc(sizeof(char*) + 2 * sizeof(size_t));
    out_p_string_obj->str    = tmp_str_p;
    out_p_string_obj->length = actual_size;
    out_p_string_obj->size   = allocated_size;
    return Ok(out_p_string_obj);
}

Result_void_p String_renew(String* string_obj_p, const char* new_format, ...)
{
    if ((string_obj_p == NULL) || (string_obj_p->str == NULL)){
        return Err(NULL, "The provided string points to NULL.", -1);
    }
    va_list args;
    char* tmp_str_p = NULL;
    va_start(args, new_format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (vasprintf(&tmp_str_p, new_format, args) == -1)
    {
        return Err(NULL, "Failed to parse format.", errno);
    }
    size_t new_len = strlen(tmp_str_p);

    if (new_len >= string_obj_p->size)
    {
        string_obj_p->str  = (char*)reallocf(string_obj_p->str, sizeof(char) * new_len * 2);
        string_obj_p->size = new_len * 2;
    }
    // Copy an extra byte for the NULL characther.
    printf("Old string: %s\n", string_obj_p->str);
    printf("New string: %s\n", tmp_str_p);
    strncpy(string_obj_p->str, tmp_str_p, new_len + 1);
    free(tmp_str_p);
    string_obj_p->length = new_len;
    return Ok(NULL);
}

void String_destroy(String* string_obj_p)
{
    free(string_obj_p->str);
    string_obj_p->str = NULL;
    free(string_obj_p);
    string_obj_p = NULL;
}

Result_void_p String_print(const String* string_obj_p)
{
    if (string_obj_p == NULL)
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
    if (string_obj_p == NULL)
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

bool String_starts_with(String* p_string, const char* prefix)
{
    if (strstr(p_string->str, prefix) == p_string->str)
    {
        return true;
    }
    else
    {
        return false;
    }
}