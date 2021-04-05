#include "common.h"
#include "logger.h"

void test_logger()
{
    PRINT_BANNER("Logger")

    LOG(TRACE, "Test", NULL)
    LOG(TRACE, "Test with format %d", 23)
    LOG(TRACE, "Test with format %d %s", 23, "Hello")
    LOG(DEBUG, "Test", NULL)
    LOG(INFO, "Test", NULL)
    LOG(WARN, "Test", NULL)
    LOG(ERROR, "Test", NULL)
    printf("All good.\n");
}