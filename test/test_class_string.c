#include "class_string.h"
PRINT_BANNER("String class")

const char* str = "Hello";
String s        = String_new(str);
ASSERT_EQ(s.length, strlen(str), "Length correct.");
ASSERT_EQ(s.size, strlen(str) * 2, "Size correct.");
int res = String_println(&s);
ASSERT_EQ(res, 0, "Printing functions work.")

    // String s1 = String_new("Then number is %d", 5);
    // String_println(&s1);
    // String_println(&s1);
    // String_println(&s1);

    // String_renew(&s, "HelloHello");
    // ASSERT_EQ(s.len, 2 * strlen(str), "Length correct");

    // String_display((const String*)&s);
    // String_print((const String*)&s);

    String_destroy(&s);