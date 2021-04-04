#include "class_string.h"
#include <unistd.h>

void test_class_string()
{
    size_t counter = 0;
    PRINT_BANNER("String class")

    PRINT_TEST_TITLE("New from string");
    const char* str = "Hello everybody";
    String s1       = String_new(str);

    ASSERT_EQ(s1.length, strlen(str), "Length correct.");
    ASSERT_EQ(s1.size, strlen(str) * 2, "Size correct.");

    ASSERT_EQ(String_println(&s1), 0, "Printing functions work.")
    ASSERT_EQ(String_display(&s1), 0, "Display functions work.")

    PRINT_TEST_TITLE("New from formatter");
    const char* format1 = "The number is";
    String s2           = String_new(format1);
    size_t initial_len  = strlen(s2.str);
    size_t initial_size = initial_len * 2;
    ASSERT_EQ(s2.length, initial_len, "Length correct.");
    ASSERT_EQ(s2.size, initial_size, "Size correct.");

    ASSERT_EQ(String_println(&s2), 0, "Printing functions work.")
    ASSERT_EQ(String_display(&s2), 0, "Display functions work.")

    PRINT_TEST_TITLE("Renew without resizing");
    String_renew(&s2, "The new string content.");
    ASSERT_EQ(s2.length, strlen(s2.str), "Length correct.");
    ASSERT_EQ(s2.size, initial_size, "Size correct.");

    ASSERT_EQ(String_println(&s2), 0, "Printing functions work.")
    ASSERT_EQ(String_display(&s2), 0, "Display functions work.")

    PRINT_TEST_TITLE("Renew with resizing");
    String_renew(&s2, "The new string content is way too long.");
    ASSERT_EQ(s2.length, strlen(s2.str), "Length correct.");
    ASSERT_EQ(s2.size, s2.length * 2, "Size correct.");

    ASSERT_EQ(String_println(&s2), 0, "Printing functions work.")
    ASSERT_EQ(String_display(&s2), 0, "Display functions work.")

    PRINT_TEST_TITLE("starts_with() function");
    ASSERT_EQ(String_starts_with(&s2, "The new"), true, "starts_with() works when true");
    ASSERT_EQ(String_starts_with(&s2, "new"), false, "starts_with() works when not beginning");
    ASSERT_EQ(String_starts_with(&s2, ""), true, "starts_with() second is empty");

    String_destroy(&s1);
    String_destroy(&s2);
}