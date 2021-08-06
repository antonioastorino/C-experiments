#include "converter.h"
#include "common.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

Result_int str_to_int(const char* str)
{
    int ret_int = 0;
    int sign    = 1;
    if (str == NULL)
    {
        return Err(ret_int, "Null pointer found", ERR_NULL);
    }
    if (str[0] == '-')
    {
        sign = -1;
        str++;
    }
    else if (str[0] == '+')
    {
        str++;
    }
    if (str[0] == '\0')
    {
        return Err(ret_int, "Cannot convert the provided string into an integer",
                   ERR_PARSE_STRING_TO_INT);
    }
    while (*str != '\0')
    {
        if ((*str < '0') || (*str > '9'))
        {
            char error_message[256];
            sprintf(error_message, "Cannot convert string containing `%c`", *str);
            return Err(ret_int, error_message, ERR_PARSE_STRING_TO_INT);
        }
        ret_int = ret_int * 10 + (*str - '0');
        str++;
    }
    return Ok(ret_int * sign);
}

Result_size_t str_to_size_t(const char* str)
{
    size_t ret_size_t = 0;
    if (str == NULL)
    {
        return Err(ret_size_t, "Null pointer found", ERR_NULL);
    }
    if (str[0] == '+')
    {
        str++;
    }
    if (str[0] == '\0')
    {
        return Err(ret_size_t, "Cannot convert the provided string into a size_t",
                   ERR_PARSE_STRING_TO_LONG_INT);
    }
    while (*str != '\0')
    {
        if ((*str < '0') || (*str > '9'))
        {
            char error_message[256];
            sprintf(error_message, "Cannot convert string containing `%c`", *str);
            return Err(ret_size_t, error_message, ERR_PARSE_STRING_TO_LONG_INT);
        }
        ret_size_t = ret_size_t * 10 + (*str - '0');
        str++;
    }
    return Ok(ret_size_t);
}

Result_float str_to_float(const char* str)
{
    float ret_float = 0.0f;
    int sign        = 1;
    if (str == NULL)
    {
        return Err(ret_float, "Null pointer found", ERR_NULL);
    }
    if (str[0] == '-')
    {
        sign = -1;
        str++;
    }
    else if (str[0] == '+')
    {
        str++;
    }
    if (str[0] == '\0')
    {
        return Err(ret_float, "Cannot convert the provided string into an float",
                   ERR_PARSE_STRING_TO_FLOAT);
    }
    bool dot_found   = false;
    float multiplier = 1.0f;
    while (*str != '\0')
    {
        if (*str == '.')
        {
            if (dot_found)
            {
                // This is the second dot found. Throw an error.
                return Err(ret_float, "Invalid string: too many decimal separators found",
                           ERR_PARSE_STRING_TO_FLOAT);
            }
            else
            {
                dot_found = true;
                str++;
                continue;
            }
        }
        if ((*str < '0') || (*str > '9'))
        {
            char error_message[256];
            sprintf(error_message, "Cannot convert string containing `%c`", *str);
            return Err(ret_float, error_message, ERR_PARSE_STRING_TO_FLOAT);
        }

        ret_float = ret_float * 10.0f + (float)(*str - '0');

        if (dot_found)
        {
            multiplier *= 0.1f;
        }
        str++;
    }
    return Ok(ret_float * sign * multiplier);
}

float rounder(float to_be_rounded, float step, size_t num_of_decimals)
{
    int to_be_rounded_int;
    int step_int;
    for (size_t i = 0; i < num_of_decimals; i++)
    {
        to_be_rounded *= 10.0f;
        step *= 10.0f;
    }
    to_be_rounded_int = (int)to_be_rounded;
    step_int          = (int)step;

    int remainder = to_be_rounded_int % step_int;
    int adjuster;
    if (to_be_rounded > 0)
    {
        adjuster = (float)remainder / step > 0.5 ? step_int : (int)0;
    }
    else
    {
        adjuster = (float)remainder / step < -0.5 ? -step_int : (int)0;
    }
    int quotient_int     = (to_be_rounded_int / step_int) * step_int + adjuster;
    float quotient_float = (float)quotient_int;
    for (size_t i = 0; i < num_of_decimals; i++)
    {
        quotient_float /= 10.0f;
    }
    return quotient_float;
}

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