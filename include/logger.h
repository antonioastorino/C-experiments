#ifndef LOGGER_H
#define LOGGER_H
#include "class_string.h"
// This is where the logging level is selected.
#define LOG_LEVEL 1
#define MEM_ANALYSIS 1

#define LOG_OUT stdout
#define LOG_ERR stdout

#include <ctype.h>
#include <string.h>
#include <time.h>
// Remove full path from file name.
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// Log level definition.
#define LOG_LEVEL_TRACE 5
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 1

void get_datetime(char[], size_t);

#define LOG(level, args...)                                                                        \
    if (LOG_LEVEL >= LOG_LEVEL_##level)                                                            \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        char log_message[1024];                                                                    \
        if (snprintf(log_message, 1024, args) > 1023)                                              \
        {                                                                                          \
            fprintf(LOG_ERR, "WARN: the following log message is longer "                          \
                             "that maximum allowed (1024 bytes).\n");                              \
        }                                                                                          \
        if ((#level[0] == 'W') || (#level[0] == 'E'))                                              \
        {                                                                                          \
            fprintf(LOG_ERR, "%s - " #level "- %s:%d - %s.\n", datetime, __FILENAME__, __LINE__,   \
                    log_message);                                                                  \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            fprintf(LOG_OUT, "%s - " #level "- %s:%d - %s.\n", datetime, __FILENAME__, __LINE__,   \
                    log_message);                                                                  \
        }                                                                                          \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
    }

#endif