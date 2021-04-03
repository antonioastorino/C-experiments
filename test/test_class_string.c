#include "class_string.h"
#include <unistd.h>
PRINT_BANNER("String class")

const char* str = "Hello everybody";
String s1       = String_new(str);

ASSERT_EQ(s1.length, strlen(str), "Length correct.");
ASSERT_EQ(s1.size, strlen(str) * 2, "Size correct.");

ASSERT_EQ(String_println(&s1), 0, "Printing functions work.")
String_display(&s1);
ASSERT_EQ(String_println(&s1), 0, "Display functions work.")

const char* format1 = "The number is %d";
String s2           = String_new(format1, 400);
ASSERT_EQ(s2.length, strlen(str), "Length correct.");
ASSERT_EQ(s2.size, strlen(str) * 2, "Size correct.");

ASSERT_EQ(String_println(&s2), 0, "Printing functions work.")
String_display(&s2);
ASSERT_EQ(String_println(&s2), 0, "Display functions work.")
String_destroy(&s1);
String_destroy(&s2);
