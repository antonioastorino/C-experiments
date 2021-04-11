#include "converter.h"

#if TEST == 1
void test_converter()
{
    PRINT_BANNER("Converter")

    PRINT_TEST_TITLE("Valid to-int conversions");
    ASSERT_EQ(unwrap(str_to_int("12345")), 12345, "Integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0")), 0, "Zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-0")), 0, "Negative zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-1")), -1, "Negative integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("+1")), +1, "Positive integer with sign successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0002")), 2, "Positive integer with leading zeros converted");

    PRINT_TEST_TITLE("Invalid to-int conversions");
    ASSERT_EQ(unwrap_err(str_to_int("1f5")).code, -1, "Invalid string detected");
    ASSERT_EQ(unwrap_err(str_to_int("+-05")).code, -1, "Invalid string detected");
    ASSERT_EQ(unwrap_err(str_to_int(NULL)).code, -1, "Null string detected");
    ASSERT_EQ(unwrap_err(str_to_int("")).code, -1, "Empty string detected");

    PRINT_TEST_TITLE("Valid to-float conversions");
    printf("%f\n", unwrap(str_to_float("12.345")));
    ASSERT_EQ(unwrap(str_to_float("12.345")), 12.345f, "Float successfully converted");
    ASSERT_EQ(unwrap(str_to_float("-12.345")), -12.345f, "Negative float successfully converted");

    PRINT_TEST_TITLE("Invalid to-float conversions");
    ASSERT_EQ(unwrap_err(str_to_float("1.5.4")).code, -4, "Too many dots detected");
    ASSERT_EQ(unwrap_err(str_to_float("1r5r4")).code, -1, "Invalid string detected");
}
#endif