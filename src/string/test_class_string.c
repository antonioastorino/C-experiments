#include "class_string.h"
#include "common.h"

#if TEST == 1
void test_class_string()
{
    Result_void_p res_void;
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

    PRINT_TEST_TITLE("Trying to join a null pointer array");
    const char** null_ptr = NULL;
    test_string_p         = String_join(null_ptr, "hello");
    ASSERT_EQ(test_string_p == NULL, true, "No string created");

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
