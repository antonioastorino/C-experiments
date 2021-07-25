#include "class_string.h"
#include "common.h"

#if TEST == 1
void test_class_string()
{
    Result_void_p res_void;
    String* test_string_p;
    PRINT_BANNER();

    PRINT_TEST_TITLE("New from string")
    const char* str = "Hello everybody";
    test_string_p   = String_new(str);
    ASSERT_EQ(test_string_p->length, strlen(str), "Length correct.")
    ASSERT_EQ(test_string_p->size, (size_t)(strlen(str) * 1.5), "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")
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
    ASSERT_EQ(test_string_p->length, initial_length, "Length correct.")
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.")
    ASSERT_EQ(unwrap(String_println(test_string_p)), NULL, "Printing functions work.")

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(test_string_p, "Old string"), true,
              "starts_with() works when true")
    ASSERT_EQ(String_starts_with(test_string_p, "new"), false, "starts_with() works when false")
    ASSERT_EQ(String_starts_with(test_string_p, ""), true,
              "starts_with() works when needle is empty")
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("clone() function")
    String* test_origin = String_new("Original");
    String* test_clone  = String_clone(test_origin);
    size_t length       = test_origin->length;
    size_t size         = test_origin->size;
    ASSERT_EQ(strcmp((const char*)test_origin->str, (const char*)test_clone->str), 0,
              "Strings match.")
    String_destroy(test_origin); // The clone should still be alive.
    ASSERT_EQ(length, test_clone->length, "Size copied")
    ASSERT_EQ(size, test_clone->size, "Size copied")
    String_destroy(test_clone);

    PRINT_TEST_TITLE("replace_char() function - replace with nothing.");
    test_string_p = String_new("Some text to be modified.");
    String_println(test_string_p);
    unwrap(String_replace_char(test_string_p, ' ', '\0'));
    ASSERT_EQ(strcmp(test_string_p->str, "Sometexttobemodified."), 0, "Needles removed correctly.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, 't', '_'));
    ASSERT_EQ(strcmp(test_string_p->str, "Some_ex__obemodified."), 0, "Needles replaced correctly.")
    String_println(test_string_p);

    unwrap(String_replace_char(test_string_p, '&', '^'));
    ASSERT_EQ(strcmp(test_string_p->str, "Some_ex__obemodified."), 0,
              "String*unchanged - needle not found.")
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

    const char* input_string = "This string contains a \":pattern:\" to be found";

    String* pattern_string_p;
    pattern_string_p = unwrap(String_between_patterns(input_string, "\":", ":\""));
    ASSERT_EQ(strcmp(pattern_string_p->str, "pattern"), 0, "Pattern found in C string");
    String_destroy(pattern_string_p);

    test_string_p    = String_new(input_string);
    pattern_string_p = unwrap(String_between_patterns(test_string_p, "\":", ":\""));
    ASSERT_EQ(strcmp(pattern_string_p->str, "pattern"), 0, "Pattern found String");
    String_destroy(pattern_string_p);

    Result_String_p res_pattern_not_found = String_between_patterns(test_string_p, "--", ":\"");
    ASSERT_EQ(res_pattern_not_found.err.code, ERR_NOT_FOUND, "Prefix not found");

    res_pattern_not_found = String_between_patterns(test_string_p, "\":", "--");
    ASSERT_EQ(res_pattern_not_found.err.code, ERR_NOT_FOUND, "Prefix not found");

    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with empty string");
    test_string_p = String_new("This pattern contains \\r\\n to be removed");
    String_replace_pattern(test_string_p, "\\r\\n ", "");
    ASSERT_EQ(strcmp("This pattern contains to be removed", test_string_p->str), 0,
              "Pattern deleted");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with short string");
    test_string_p = String_new("This pattern contains \\r\\n to be replaced");
    String_replace_pattern(test_string_p, "\\r\\n", "HELLO WORLD");
    ASSERT_EQ(strcmp("This pattern contains HELLO WORLD to be replaced", test_string_p->str), 0,
              "Pattern replaced");
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("Pattern replacement with long string");
    test_string_p = String_new("This pattern contains \\r\\n to be replaced");
    String_replace_pattern(test_string_p, "\\r\\n", "HELLO WORLD! This is the replacement pattern");
    ASSERT_EQ(
        strcmp("This pattern contains HELLO WORLD! This is the replacement pattern to be replaced",
               test_string_p->str),
        0, "Pattern replaced");
    String_destroy(test_string_p);
}
#endif
