#include "class_string.h"
#include <unistd.h>

void test_class_string()
{
    size_t counter = 0;
    Result_void result;
    PRINT_BANNER("String class")

    PRINT_TEST_TITLE("New from string");
    const char* str = "Hello everybody";
    String test_string;
    (void)String_new(&test_string, str);

    ASSERT_EQ(test_string.length, strlen(str), "Length correct.");
    ASSERT_EQ(test_string.size, strlen(str) * 2, "Size correct.");
    result = String_println(&test_string);
    ASSERT_EQ(result.error_code, 0, "Printing functions work.")
    result = String_display(&test_string);
    ASSERT_EQ(result.error_code, 0, "Display functions work.")
    String_destroy(&test_string);

    PRINT_TEST_TITLE("New from formatter");
    const char* format1 = "The number is";
    (void)String_new(&test_string, format1);
    size_t initial_len  = strlen(test_string.str);
    size_t initial_size = initial_len * 2;
    ASSERT_EQ(test_string.length, initial_len, "Length correct.");
    ASSERT_EQ(test_string.size, initial_size, "Size correct.");
    result = String_println(&test_string);
    ASSERT_EQ(result.error_code, 0, "Printing functions work.")
    result = String_display(&test_string);
    ASSERT_EQ(result.error_code, 0, "Display functions work.")
    String_destroy(&test_string);

    PRINT_TEST_TITLE("Renew without resizing");
    String_renew(&test_string, "The new string content.");
    ASSERT_EQ(test_string.length, strlen(test_string.str), "Length correct.");
    ASSERT_EQ(test_string.size, initial_size, "Size correct.");
    result = String_println(&test_string);
    ASSERT_EQ(result.error_code, 0, "Printing functions work.")
    result = String_display(&test_string);
    ASSERT_EQ(result.error_code, 0, "Display functions work.")
    String_destroy(&test_string);

    PRINT_TEST_TITLE("Renew with resizing");
    String_renew(&test_string, "The new string content is way too long.");
    ASSERT_EQ(test_string.length, strlen(test_string.str), "Length correct.");
    ASSERT_EQ(test_string.size, test_string.length * 2, "Size correct.");
    result = String_println(&test_string);
    ASSERT_EQ(result.error_code, 0, "Printing functions work.")
    result = String_display(&test_string);
    ASSERT_EQ(result.error_code, 0, "Display functions work.")
    String_destroy(&test_string);

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(&test_string, "The new"), true, "starts_with() works when true");
    ASSERT_EQ(String_starts_with(&test_string, "new"), false,
              "starts_with() works when not beginning");
    ASSERT_EQ(String_starts_with(&test_string, ""), true, "starts_with() second is empty");

    String_destroy(&test_string);
}