#include "class_string.h"
#include <unistd.h>

void test_class_string()
{
    size_t counter = 0;
    Result_void_p res_void;
    String* test_string_p;
    PRINT_BANNER("String class")

    PRINT_TEST_TITLE("New from string");
    const char* str = "Hello everybody";
    test_string_p   = unwrap_String_p(String_new(str));
    ASSERT_EQ(test_string_p->length, strlen(str), "Length correct.");
    ASSERT_EQ(test_string_p->size, strlen(str) * 2, "Size correct.");
    ASSERT_EQ(unwrap_void_p(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap_void_p(String_display(test_string_p)), NULL, "Display functions work.")
    String_destroy(test_string_p);

    PRINT_TEST_TITLE("New from formatter");
    const char* format1   = "The number is";
    test_string_p         = unwrap_String_p(String_new(format1));
    size_t initial_length = strlen(test_string_p->str);
    size_t initial_size   = initial_length * 2;
    ASSERT_EQ(test_string_p->length, initial_length, "Length correct.");
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.");
    ASSERT_EQ(unwrap_void_p(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap_void_p(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("Renew without resizing");
    unwrap_void_p(String_renew(test_string_p, "The new string content."));
    ASSERT_EQ(test_string_p->length, strlen(test_string_p->str), "Length correct.");
    ASSERT_EQ(test_string_p->size, initial_size, "Size correct.");
    ASSERT_EQ(unwrap_void_p(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap_void_p(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("Renew with resizing");
    (String_renew(test_string_p, "The new string content is way too long."));
    ASSERT_EQ(test_string_p->length, strlen(test_string_p->str), "Length correct.");
    ASSERT_EQ(test_string_p->size, test_string_p->length * 2, "Size correct.");
    ASSERT_EQ(unwrap_void_p(String_println(test_string_p)), NULL, "Printing functions work.")
    ASSERT_EQ(unwrap_void_p(String_display(test_string_p)), NULL, "Display functions work.")

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(test_string_p, "The new"), true, "starts_with() works when true");
    ASSERT_EQ(String_starts_with(test_string_p, "new"), false, "starts_with() works when false");
    ASSERT_EQ(String_starts_with(test_string_p, ""), true,
              "starts_with() works when needle is empty");
    String_destroy(test_string_p);
}