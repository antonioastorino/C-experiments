#ifndef LOGGER_H
#define LOGGER_H

// This is where the logging level is selected.
#define LOG_LEVEL 5

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

void to_uppercase(char[]);

#if LOG_LEVEL >= LOG_LEVEL_TRACE
#define LOG_TRACE(message)                                                                         \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - TRACE - %s:%d - %s\n", datetime, __FILENAME__, __LINE__, message);            \
    }
#else
#define LOG_TRACE(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(message)                                                                         \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - DEBUG - %s:%d - %s\n", datetime, __FILENAME__, __LINE__, message);            \
    }
#else
#define LOG_DEBUG(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(message)                                                                          \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - INFO - %s:%d - %s\n", datetime, __FILENAME__, __LINE__, message);             \
    }
#else
#define LOG_INFO(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(message)                                                                          \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - WARN - %s:%d - %s\n", datetime, __FILENAME__, __LINE__, message);             \
    }
#else
#define LOG_WARN(message)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(message)                                                                         \
    {                                                                                              \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - ERROR - %s:%d - %s\n", datetime, __FILENAME__, __LINE__, message);            \
    }
#else
#define LOG_ERROR(message)
#endif

#endif
