#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

#if TEST == 1
void test_logger()
{
    PRINT_BANNER("Logger")

    LOG_TRACE("Test")
    LOG_TRACE("Test with format %d", 23)
    LOG_TRACE("Test with format %d %s", 23, "Hello")
    LOG_DEBUG("Test")
    LOG_INFO("Test")
    LOG_WARN("Test")
    LOG_ERROR("Test")
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