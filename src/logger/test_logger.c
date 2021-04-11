#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

#if TEST == 1
void test_logger()
{
    PRINT_BANNER("Logger")

    LOG(TRACE, "Test")
    LOG(TRACE, "Test with format %d", 23)
    LOG(TRACE, "Test with format %d %s", 23, "Hello")
    LOG(DEBUG, "Test")
    LOG(INFO, "Test")
    LOG(WARN, "Test")
    LOG(ERROR, "Test")
    LOG(ERROR,
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