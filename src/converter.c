#include "converter.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

Result_int str_to_int(char* str)
{
    int ret_int = 0;
    int sign    = 1;
    if (str == NULL)
    { // TODO: create error code
        return Err(ret_int, "Null pointer found", -1);
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
    { // TODO: create error code
        return Err(ret_int, "Cannot convert the provided string into an integer", -1);
    }
    while (*str != '\0')
    {
        if ((*str < '0') || (*str > '9'))
        {
            char error_message[256];
            sprintf(error_message, "Cannot convert string containing `%c`", *str);
            return Err(ret_int, error_message, -1);
        }
        ret_int = ret_int * 10 + (*str - '0');
        str++;
    }
    return Ok(ret_int * sign);
}

#if TEST == 1
void test_converter()
{
    PRINT_BANNER("Converter")

    PRINT_TEST_TITLE("Valid conversions");
    ASSERT_EQ(unwrap(str_to_int("12345")), 12345, "Integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0")), 0, "Zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-0")), 0, "Negative zero successfully converted");
    ASSERT_EQ(unwrap(str_to_int("-1")), -1, "Negative integer successfully converted");
    ASSERT_EQ(unwrap(str_to_int("+1")), +1, "Positive integer with sign successfully converted");
    ASSERT_EQ(unwrap(str_to_int("0002")), 2, "Positive integer with leading zeros converted");

    PRINT_TEST_TITLE("Invalid conversions");
    ASSERT_EQ(unwrap_err(str_to_int("1f5")).code, -1, "Invalid string detected");
    ASSERT_EQ(unwrap_err(str_to_int("+-05")).code, -1, "Invalid string detected");
    ASSERT_EQ(unwrap_err(str_to_int(NULL)).code, -1, "Null string detected");
    ASSERT_EQ(unwrap_err(str_to_int("")).code, -1, "Empty string detected");
}
#endif