#pragma once

#include "core/defines.h"
#include "core/string.h"

#include <stdio.h>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

typedef enum log_level_t {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
} log_level_t; 

void log_assert_fail(const char* expression, const char* message, const char* file, i32 line);

void _log_output(log_level_t level, const char* format, ...);

#define LOG_FATAL(msg, ...) _log_output(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)

#define LOG_ERROR(msg, ...) _log_output(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)

#if LOG_WARN_ENABLED == 1
#define LOG_WARN(msg, ...) _log_output(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#else
#define LOG_WARN(msg, ...)
#endif // LOG_WARN_ENABLED

#if LOG_INFO_ENABLED == 1
#define LOG_INFO(msg, ...) _log_output(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#else
#define LOG_INFO(msg, ...)
#endif // LOG_INFO_ENABLED

#if LOG_DEBUG_ENABLED == 1
#define LOG_DEBUG(msg, ...) _log_output(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define LOG_DEBUG(msg, ...)
#endif // LOG_DEBUG_ENABLED

#if LOG_TRACE_ENABLED == 1
#define LOG_TRACE(msg, ...) _log_output(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
#define LOG_TRACE(msg, ...)
#endif // LOG_TRACE_ENABLED
