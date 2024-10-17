#include "core/logger.h"

#include "core/defines.h"
#include "core/string.h"
#include "core/str.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void _log_output(log_level_t level, const char* message, ...) {
    const char* level_literals[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

    string_t out_message = string_create_with_capacity(0, 64);
    string_t formatted = string_create_with_capacity(0, 64);

    va_list args;
    va_start(args, message);
    string_append_format_v(formatted, message, args);
    va_end(args);

    string_append_format(out_message, "%s%s\n", level_literals[level], formatted);
    /*string_destroy(formatted);*/

    printf(out_message);

    string_destroy(out_message);

    if (level == LOG_LEVEL_FATAL) {
        exit(1);
    }
}

void log_assert_fail(const char* expression, const char* message, const char* file, i32 line) {
    _log_output(LOG_LEVEL_FATAL, "Assert fail: %s, message: '%s', (file:line): %s:%d\n", expression, message, file, line);
}
