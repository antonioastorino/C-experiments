#include "class_string.h"
#include "logger.h"
#include "mem.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_FACTOR 1.5

bool String_is_null(const String* string_obj_p)
{
    if (string_obj_p == NULL)
        return true;
    if (string_obj_p->str == NULL)
        return true;
    return false;
}

String* String_new(const char* format, ...)
{
    va_list args;
    char* tmp_str_p = NULL;
    String* out_string_obj_p;
    va_start(args, format);
    // Calculate how many bytes are needed (excluding the terminating '\0').
    if (VASPRINTF(&tmp_str_p, format, args) == -1)
    {
        LOG_ERROR("Out of memory", errno)
        exit(ERR_NULL);
    }
    size_t actual_size = strlen(tmp_str_p);
    // Allocate twice the required length
    size_t allocated_size = (size_t)(actual_size * SIZE_FACTOR);
    // printf("Allocated size: %zu\n", allocated_size);
    tmp_str_p = (char*)REALLOCF(tmp_str_p, sizeof(char) * allocated_size);
    LOG_TRACE("Created string.")
    va_end(args);
    // Set the `.len` parameter as the length of the string, excluding the terminating '\0'.
    out_string_obj_p         = (String*)MALLOC(sizeof(String));
    out_string_obj_p->str    = tmp_str_p;
    out_string_obj_p->length = actual_size;
    out_string_obj_p->size   = allocated_size;
    return out_string_obj_p;
}

// The array must end with a NULL value.
String* String_join(const char** char_array, const char* joint)
{
    if (!char_array || !char_array[0])
    {
        LOG_ERROR("Please provide a valid input array.");
        return (String*)NULL;
    }
    const char** curr_element = char_array;
    String* ret_string_p      = String_new(*curr_element);
    while (*(curr_element + 1) != NULL)
    {
        // TODO: Use String_renew() instead.
        String* new_ret_string_p
            = String_new("%s%s%s", ret_string_p->str, joint, *(++curr_element));
        String_destroy(ret_string_p);
        ret_string_p = new_ret_string_p;
    }
    return ret_string_p;
}

String* String_clone(const String* origin) { return String_new(origin->str); }

void String_destroy(String* string_obj_p)
{
    FREE(string_obj_p->str);
    string_obj_p->str    = 0;
    string_obj_p->length = -1;
    string_obj_p->size   = -1;

    FREE(string_obj_p);
}

Result_void_p String_print(const String* string_obj_p)
{
    if (String_is_null(string_obj_p))
    {
        return Err(NULL, "Uninitialized string.", ERR_NULL);
    }
    for (size_t i = 0; i < string_obj_p->length; i++)
    {
        printf("%c", string_obj_p->str[i]);
    }
    fflush(stdout);
    return Ok(NULL);
}

Result_void_p String_println(const String* string_obj_p)
{
    Result_void_p result = String_print(string_obj_p);
    RET_ON_ERR(NULL, result);
    printf("\n");
    return result;
}

// TODO: Change to Result_bool.
bool String_starts_with(const String* string_p, const char* prefix)
{
    if (String_is_null(string_p))
    {
        LOG_ERROR("Trying to check the start of an empty string");
        return false;
    }
    if (strstr(string_p->str, prefix) == string_p->str)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool String_match(const String* a_string_p, const String* b_string_p)
{
    if (a_string_p == NULL || b_string_p == NULL)
    {
        LOG_WARN("Input strings should not be NULL");
        return false;
    }
    if (strcmp(a_string_p->str, b_string_p->str))
    {
        return false;
    }
    return true;
}

Result_size_t String_replace_char(String* haystack_string_p, const char needle, const char replace)
{
    if (String_is_null(haystack_string_p))
    {
        return Err((size_t)0, "Uninitialized string.", -1);
    }
    char tmp_str[haystack_string_p->length + 1];
    size_t i = 0, j = 0, cnt = 0;
    while (i < haystack_string_p->length)
    {
        if (haystack_string_p->str[i] == needle)
        {
            cnt++;
            if (replace != '\0')
            {
                // Replace the current char with that provided.
                tmp_str[j++] = replace;
            }
        }
        else
        {
            tmp_str[j++] = haystack_string_p->str[i];
        }
        i++;
    }
    // Terminate.
    tmp_str[j] = '\0';
    // Update the string length in case some chars were removed.
    haystack_string_p->length = j;
    strncpy(haystack_string_p->str, tmp_str, j + 1);
    return Ok(cnt);
}

Result_size_t String_replace_pattern_size_t(String* haystack_string_p, const char* needle,
                                            const char* format, size_t replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_size_t ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_size_t String_replace_pattern_float(String* haystack_string_p, const char* needle,
                                           const char* format, float replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_size_t ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_size_t String_replace_pattern_int(String* haystack_string_p, const char* needle,
                                         const char* format, int replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_size_t ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_size_t String_replace_pattern(String* haystack_string_p, const char* needle,
                                     const char* replacement)
{

    if (String_is_null(haystack_string_p))
    {
        return Err((size_t)0, "Uninitialized string.", ERR_NULL);
    }
    if (strlen(needle) == 0)
    {
        return Err((size_t)0, "Empty needle not allowed.", ERR_EMPTY_STRING);
    }
    int oldWlen = strlen(needle);
    int newWlen = strlen(replacement);
    size_t i;
    size_t cnt = 0;

    // Counting the number of times old word occur in the string
    const char* s = haystack_string_p->str;
    for (i = 0; i < haystack_string_p->length; i++)
    {
        if (strstr(&s[i], needle) == &s[i])
        {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
    if (cnt == 0)
    {
        LOG_DEBUG("Pattern %s not found", needle);
        return Ok((size_t)0);
    }

    // Making new string of enough length
    size_t new_string_length         = i + cnt * (newWlen - oldWlen);
    char* result_char_p              = (char*)MALLOC(new_string_length + 1);
    result_char_p[new_string_length] = 0;

    i = 0;
    while (*s)
    {
        // compare the substring with the result
        if (strstr(s, needle) == s)
        {
            strcpy(&result_char_p[i], replacement);
            i += newWlen;
            s += oldWlen;
        }
        else
            result_char_p[i++] = *s++;
    }

    // Update string.
    if (new_string_length >= haystack_string_p->size)
    {
        // Increase the allocated size.
        haystack_string_p->size = (size_t)(new_string_length * SIZE_FACTOR);
        haystack_string_p->str  = (char*)REALLOCF(haystack_string_p->str, haystack_string_p->size);
    }
    haystack_string_p->length = new_string_length;
    // Copy an extra byte for the NULL characther.
    strncpy(haystack_string_p->str, result_char_p, new_string_length + 1);
    haystack_string_p->str[new_string_length] = 0;
    FREE(result_char_p);
    result_char_p = NULL;

    return Ok(cnt);
}

Result_String_p String_between_patterns_in_char_p(const char* in_char_p, const char* prefix,
                                                  const char* suffix)
{
    String* ret_string_p;
    if (in_char_p == NULL)
    {
        return Err(ret_string_p, "Uninitialized input detected.", ERR_NULL)
    }
    else if (strlen(in_char_p) < strlen(prefix) + strlen(suffix))
    {
        LOG_DEBUG("Input str length : %lu", strlen(in_char_p));
        LOG_DEBUG("Prefix length    : %lu", strlen(in_char_p));
        LOG_DEBUG("Suffix length    : %lu", strlen(in_char_p));

        return Err(ret_string_p, "Input shorter than the input patterns.", ERR_INVALID_INPUT);
    }
    char* start = strstr(in_char_p, prefix);
    if (start == NULL)
    {
        return Err(ret_string_p, "Prefix not found in input string", ERR_NOT_FOUND);
    }
    start     = start + strlen(prefix);
    char* end = strstr(start, suffix);
    if (end == NULL)
    {
        return Err(ret_string_p, "Suffix not found in input string", ERR_NOT_FOUND);
    }
    char* tmp = (char*)MALLOC(end - start + 1);
    memcpy(tmp, start, end - start);
    tmp[end - start] = '\0';

    String* res_string_p = String_new(tmp);
    FREE(tmp);
    tmp = NULL;
    return Ok(res_string_p);
}

Result_String_p String_between_patterns_in_string_p(String* in_string_p, const char* prefix,
                                                    const char* suffix)
{
    return String_between_patterns_in_char_p(in_string_p->str, prefix, suffix);
}