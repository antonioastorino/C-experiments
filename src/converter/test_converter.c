#include "converter.h"

#if TEST == 1
void test_converter()
{
    PRINT_BANNER();

    PRINT_TEST_TITLE("Valid to-int conversions");
    ASSERT_EQ(unwrap(str_to_int("12345")), 12345, "Integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0")), 0, "Zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-0")), 0, "Negative zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-1")), -1, "Negative integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("+1")), +1, "Positive integer with sign successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0002")), 2, "Positive integer with leading zeros converted");

    PRINT_TEST_TITLE("Invalid to-int conversions");
    ASSERT_EQ((int)str_to_int("1f5").err.code, ERR_PARSE_STRING_TO_INT, "Invalid string detected");
    ASSERT_EQ((int)str_to_int("+-05").err.code, ERR_PARSE_STRING_TO_INT, "Invalid string detected");
    ASSERT_EQ((int)str_to_int(NULL).err.code, ERR_NULL, "Null string detected");
    ASSERT_EQ((int)str_to_int("").err.code, ERR_PARSE_STRING_TO_INT, "Empty string detected");

    PRINT_TEST_TITLE("Valid to-size_t conversions");
    ASSERT_EQ(unwrap(str_to_size_t("1234567890123")), 1234567890123,
              "size_t successfully converted");
    ASSERT_EQ(unwrap(str_to_size_t("0")), 0, "Zero successfully converted");
    ASSERT_EQ(unwrap(str_to_size_t("+1")), +1, "size_t with sign successfully converted");
    ASSERT_EQ(unwrap(str_to_size_t("0002")), 2, "size_t with leading zeros converted");

    PRINT_TEST_TITLE("Invalid to-int conversions");
    ASSERT_EQ((int)str_to_size_t("1f5").err.code, ERR_PARSE_STRING_TO_LONG_INT,
              "Invalid string detected");
    ASSERT_EQ((int)str_to_size_t("-235").err.code, ERR_PARSE_STRING_TO_LONG_INT,
              "Invalid string detected");
    ASSERT_EQ((int)str_to_size_t("+-05").err.code, ERR_PARSE_STRING_TO_LONG_INT,
              "Invalid string detected");
    ASSERT_EQ((int)str_to_size_t(NULL).err.code, ERR_NULL, "Null string detected");
    ASSERT_EQ((int)str_to_size_t("").err.code, ERR_PARSE_STRING_TO_LONG_INT,
              "Empty string detected");

    PRINT_TEST_TITLE("Valid to-float conversions");
    printf("%f\n", unwrap(str_to_float("12.345")));
    ASSERT_EQ(unwrap(str_to_float("12.345")), 12.345f, "Float successfully converted");
    ASSERT_EQ(unwrap(str_to_float("-12.345")), -12.345f, "Negative float successfully converted");
    ASSERT_EQ(unwrap(str_to_float("-12")), -12.0f,
              "Negative float with no decimals successfully converted");
    ASSERT_EQ(unwrap(str_to_float("12")), 12.0f,
              "Positive float with no decimals successfully converted");
    ASSERT_EQ(unwrap(str_to_float("+12")), 12.0f,
              "Positive float with explicit sign and no decimals successfully converted");

    PRINT_TEST_TITLE("Invalid to-float conversions");
    ASSERT_EQ((int)str_to_float("1.5.4").err.code, ERR_PARSE_STRING_TO_FLOAT,
              "Too many dots detected");
    ASSERT_EQ((int)str_to_float("1r5r4").err.code, ERR_PARSE_STRING_TO_FLOAT,
              "Invalid string detected");

    PRINT_TEST_TITLE("Rounding");
    ASSERT_EQ(rounder(12.34f, 0.1f, 2), 12.30f, "Rounding correct.");
    ASSERT_EQ(rounder(12.34f, 1.0f, 2), 12.00f, "Rounding correct.");
    ASSERT_EQ(rounder(12.34f, 0.2f, 2), 12.40f, "Rounding UP correct.");
    ASSERT_EQ(rounder(-12.34f, 0.1f, 2), -12.30f, "Rounding correct.");
    ASSERT_EQ(rounder(-12.34f, 1.0f, 2), -12.00f, "Rounding correct.");
    ASSERT_EQ(rounder(-12.34f, 0.2f, 2), -12.40f, "Rounding UP correct.");
}
#endif