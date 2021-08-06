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