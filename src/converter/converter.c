#include "converter.h"
#include "common.h"
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

Result_float str_to_float(char* str)
{
    float ret_float = 0.0f;
    int sign        = 1;
    if (str == NULL)
    { // TODO: create error code
        return Err(ret_float, "Null pointer found", -1);
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
        return Err(ret_float, "Cannot convert the provided string into an float", -1);
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
                return Err(ret_float, "Invalid string: too many decimal separators found", -4);
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
            return Err(ret_float, error_message, -1);
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