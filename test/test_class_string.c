#include "class_string.h"
PRINT_BANNER("String class")

const char* str = "Hello";
String s        = String_new(str);
String_println(&s);
ASSERT_EQ(s.len, 2 * strlen(str), "Length correct");

String_renew(&s, "HelloHello");
ASSERT_EQ(s.len, 2 * strlen(str), "Length correct");

String_display((const String*)&s);
String_print((const String*)&s);

String_destroy(&s);