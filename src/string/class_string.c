#include "class_string.h"
#include "common.h"
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
    RETURN_ON_ERROR(NULL, result);
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

Result_uint String_replace_char(String* haystack_string_p, const char needle, const char replace)
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

Result_uint String_replace_pattern_uint(String* haystack_string_p, const char* needle,
                                        const char* format, size_t replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_uint ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_uint String_replace_pattern_float(String* haystack_string_p, const char* needle,
                                         const char* format, float replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_uint ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_uint String_replace_pattern_int(String* haystack_string_p, const char* needle,
                                       const char* format, int replacement)
{
    String* replacement_string_p = String_new(format, replacement);
    Result_uint ret_result
        = String_replace_pattern(haystack_string_p, needle, replacement_string_p->str);
    String_destroy(replacement_string_p);
    return ret_result;
}

Result_uint String_replace_pattern(String* haystack_string_p, const char* needle,
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

#if TEST == 1
void test_class_string()
{
    size_t num_of_replacements;
    String* test_string_p;
    PRINT_BANNER();

    PRINT_TEST_TITLE("New from string")
    const char* str = "Hello everybody";
    test_string_p   = String_new(str);
    ASSERT_EQ(test_string_p->length, strlen(str), "Length correct.");
    ASSERT_EQ(test_string_p->size, (size_t)(strlen(str) * 1.5), "Size correct.");
    ASSERT_EQ((int)String_println(test_string_p).err.code, ERR_ALL_GOOD,
              "Printing functions work.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Check is NULL");
    ASSERT_EQ(String_is_null(test_string_p), true, "Already destroyed.");
    test_string_p = NULL;
    ASSERT_EQ(String_is_null(test_string_p), true, "String pointer set to NULL.");

    PRINT_TEST_TITLE("New from formatter");
    const char* format1   = "Old string content.";
    test_string_p         = String_new(format1);
    size_t initial_length = strlen(test_string_p->str);
    size_t initial_size   = (size_t)(initial_length * 1.5);
    ASSERT_EQ(test_string_p->length, initial_length, "Length correct.");
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.");

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(test_string_p, "Old string"), true,
              "starts_with() works when true");
    ASSERT_EQ(String_starts_with(test_string_p, "new"), false, "starts_with() works when false");
    ASSERT_EQ(String_starts_with(test_string_p, ""), true,
              "starts_with() works when needle is empty");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("clone() function")
    String* test_origin = String_new("Original");
    String* test_clone  = String_clone(test_origin);
    size_t length       = test_origin->length;
    size_t size         = test_origin->size;
    ASSERT_EQ(test_origin->str, test_clone->str, "Strings match.");
    String_destroy(test_origin); // The clone should still be alive.
    ASSERT_EQ(length, test_clone->length, "Size copied");
    ASSERT_EQ(size, test_clone->size, "Size copied");
    String_destroy(test_clone);

    PRINT_TEST_TITLE("replace_char() function - replace with nothing.");
    test_string_p = String_new("Some text to be modified.");
    String_println(test_string_p);
    num_of_replacements = unwrap(String_replace_char(test_string_p, ' ', '\0'));
    ASSERT_EQ(test_string_p->str, "Sometexttobemodified.", "Needles removed correctly.");
    ASSERT_EQ(num_of_replacements, 4, "Replacements counted correctly.");
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, 't', '_'));
    ASSERT_EQ(test_string_p->str, "Some_ex__obemodified.", "Needles replaced correctly.");
    String_println(test_string_p);

    num_of_replacements = unwrap(String_replace_char(test_string_p, '&', '^'));
    ASSERT_EQ(test_string_p->str, "Some_ex__obemodified.", "String unchanged - needle not found.");
    ASSERT_EQ(num_of_replacements, 0, "No replacements counted correctly.");
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
    ASSERT_EQ(test_string_p->str, "+++++++++++++++++++++",
              "Replaced all chars in string with '+'.");
    String_println(test_string_p);

    initial_length      = test_string_p->length;
    num_of_replacements = unwrap(String_replace_char(test_string_p, '+', '\0'));
    ASSERT_EQ(test_string_p->str, "", "Ths string is correctly emptied.");
    ASSERT_EQ(test_string_p->length, 0, "Ths length is correctly set to 0.");
    ASSERT_EQ(num_of_replacements, initial_length, "Number of replacements counted correctly.");
    String_println(test_string_p);
    String_destroy(test_string_p);

    const char* input_string = "This string contains a \":pattern:\" to be found";

    String* pattern_string_p;
    pattern_string_p = unwrap(String_between_patterns(input_string, "\":", ":\""));
    ASSERT_EQ(pattern_string_p->str, "pattern", "Pattern found in C string");
    String_destroy(pattern_string_p);

    test_string_p    = String_new(input_string);
    pattern_string_p = unwrap(String_between_patterns(test_string_p, "\":", ":\""));
    ASSERT_EQ(pattern_string_p->str, "pattern", "Pattern found String");
    String_destroy(pattern_string_p);

    Result_String_p res_pattern_not_found = String_between_patterns(test_string_p, "--", ":\"");
    ASSERT_EQ((int)res_pattern_not_found.err.code, ERR_NOT_FOUND, "Prefix not found");

    res_pattern_not_found = String_between_patterns(test_string_p, "\":", "--");
    ASSERT_EQ((int)res_pattern_not_found.err.code, ERR_NOT_FOUND, "Prefix not found");

    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with empty string");
    test_string_p       = String_new("This pattern contains \\r\\n to be removed");
    num_of_replacements = unwrap(String_replace_pattern(test_string_p, "\\r\\n ", ""));
    ASSERT_EQ("This pattern contains to be removed", test_string_p->str, "Pattern deleted");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with short string");
    test_string_p       = String_new("This pattern contains \\r\\n to be replaced");
    num_of_replacements = unwrap(String_replace_pattern(test_string_p, "\\r\\n", "HELLO WORLD"));
    ASSERT_EQ("This pattern contains HELLO WORLD to be replaced", test_string_p->str,
              "Pattern replaced");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with float");
    test_string_p = String_new("This pattern contains \\r\\n to be replaced");
    num_of_replacements
        = unwrap(String_replace_pattern_with_format(test_string_p, "\\r\\n", "%.4f", 10.3f));
    ASSERT_EQ("This pattern contains 10.3000 to be replaced", test_string_p->str,
              "Pattern replaced");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with size_t");
    test_string_p = String_new("This pattern contains \\r\\n to be replaced");
    num_of_replacements
        = unwrap(String_replace_pattern_with_format(test_string_p, "\\r\\n", "%lu", (size_t)1003));
    ASSERT_EQ("This pattern contains 1003 to be replaced", test_string_p->str, "Pattern replaced");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with int");
    test_string_p = String_new("This pattern contains \\r\\n to be replaced");
    num_of_replacements
        = unwrap(String_replace_pattern_with_format(test_string_p, "\\r\\n", "%d", -1003));
    ASSERT_EQ("This pattern contains -1003 to be replaced", test_string_p->str, "Pattern replaced");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with long string");
    test_string_p       = String_new("This pattern contains \\r\\n to be replaced");
    num_of_replacements = unwrap(String_replace_pattern(
        test_string_p, "\\r\\n", "HELLO WORLD! This is the replacement pattern"));
    ASSERT_EQ("This pattern contains HELLO WORLD! This is the replacement pattern to be replaced",
              test_string_p->str, "Pattern replaced");
    ASSERT_EQ(num_of_replacements, 1, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern not replaced because missing");
    test_string_p       = String_new("This string does not contain a pattern to be replaced");
    num_of_replacements = unwrap(String_replace_pattern(
        test_string_p, "missing pattern", "HELLO WORLD! This is the replacement pattern"));
    ASSERT_EQ("This string does not contain a pattern to be replaced", test_string_p->str,
              "Pattern not found");
    ASSERT_EQ(num_of_replacements, 0, "Number of replacements counted correctly.");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Trying to join an empty array");
    const char* char_array[] = {NULL};
    test_string_p            = String_join(char_array, "hello");
    ASSERT_EQ(test_string_p == NULL, true, "No string created");

    PRINT_TEST_TITLE("Join with 1 element");
    const char* one_element_array[] = {"element 1", NULL};
    test_string_p                   = String_join(one_element_array, "hello");
    ASSERT_EQ(test_string_p->str, "element 1", "No concatenation performed");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Join with 2 elements");
    const char* two_element_array[] = {"element 1", "element 2", NULL};
    test_string_p                   = String_join(two_element_array, "|||");
    ASSERT_EQ(test_string_p->str, "element 1|||element 2", "Concatenation correct");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Join with empty separator");
    const char* foo_bar_element_array[] = {"F", "O", "O", "B", "A", "R", NULL};
    test_string_p                       = String_join(foo_bar_element_array, "");
    ASSERT_EQ(test_string_p->str, "FOOBAR", "Concatenation correct");
    String_destroy(test_string_p);
    /**/
}
#endif
