#include "common.h"
#define PRINT_PASS_MESSAGE(message) printf("\n> \e[32mPASS\e[0m\t %s\n", message)
#define PRINT_FAIL_MESSAGE(message, filename, line_number)                                         \
    printf("\n> \e[31mFAIL\e[0m\t %s\n", message);                                                 \
    fprintf(stderr, "\n> Err - Test failed.\n%s:%d : left != right\n", filename, line_number)

void ASSERT_EQ_int(int value_1, int value_2, const char* message, const char* filename,
                   int line_number)
{
    if (value_1 == value_2)
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%d`\nRight: `%d`\n", value_1, value_2);
    }
}

void ASSERT_EQ_size_t(size_t value_1, size_t value_2, const char* message, const char* filename,
                      int line_number)
{
    if (value_1 == value_2)
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%lu`\nRight: `%lu`\n", value_1, value_2);
    }
}

void ASSERT_EQ_bool(bool value_1, bool value_2, const char* message, const char* filename,
                    int line_number)
{
    if (value_1 == value_2)
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%s`\nRight: `%s`\n", value_1 ? "true" : "false",
                value_2 ? "true" : "false");
    }
}
void ASSERT_EQ_float(float value_1, float value_2, const char* message, const char* filename,
                     int line_number)
{
    if (value_1 == value_2)
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%f`\nRight: `%f`\n", value_1, value_2);
    }
}

void ASSERT_EQ_double(double value_1, double value_2, const char* message, const char* filename,
                      int line_number)
{
    if (value_1 == value_2)
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%lf`\nRight: `%lf`\n", value_1, value_2);
    }
}

void ASSERT_EQ_char_p(const char* value_1, const char* value_2, const char* message,
                      const char* filename, int line_number)
{
    if (!strcmp(value_1, value_2))
    {
        PRINT_PASS_MESSAGE(message);
    }
    else
    {
        PRINT_FAIL_MESSAGE(message, filename, line_number);
        fprintf(stderr, "Left : `%s`\nRight: `%s`\n", value_1, value_2);
    }
}