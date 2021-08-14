#include "logger.h"
#include "common.h"
#include <stdlib.h>

#if LOG_LEVEL > 0
void get_datetime(char* datetime, const size_t buf_len)
{

    time_t now     = time(&now);
    struct tm* ptm = gmtime(&now);
    if ((ptm == NULL) || (now == -1))
    {
        datetime[0] = 0;
    }
    strftime(datetime, buf_len, "%Y %b %d %X", ptm);
}

FILE* log_out_file_p;
FILE* log_err_file_p;

FILE* get_log_out_file() { return log_out_file_p == NULL ? stdout : log_out_file_p; }
FILE* get_log_err_file() { return log_err_file_p == NULL ? stdout : log_err_file_p; }

void init_logger(const char* log_out_file_path_str, const char* log_err_file_path_str)
{
    if ((log_out_file_p != NULL) || (log_err_file_p != NULL))
    {
        perror("Fatal error: logger already initialized!");
        exit(-1);
    }

    log_out_file_p = fopen(log_out_file_path_str, "w");
    if (strcmp(log_out_file_path_str, log_err_file_path_str) != 0)
    {
        log_err_file_p = fopen(log_err_file_path_str, "w");
    }
    else
    {
        log_err_file_p = log_out_file_p;
    }
}

#if TEST == 1
void test_logger()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Logging at any level.");
    LOG_TRACE("Test")
    LOG_TRACE("Test with format %d", 23)
    LOG_TRACE("Test with format %d %s", 23, "Hello")
    LOG_DEBUG("Test")
    LOG_INFO("Test")
    LOG_WARN("Test")
    LOG_ERROR("Test")
    PRINT_TEST_TITLE("Logging too long string.");
    LOG_ERROR(
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - Text too long - Text too "
        "long - Text too long - Text too long - Text too long - Text too long - Text too long - "
        "Text too long - Text too long - Text too long - Text too long - ")
    printf("All good.\n");
}
#endif
#endif
