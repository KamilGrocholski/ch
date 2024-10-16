#pragma once

#define ASSERT(expr) { \
    if (expr) { \
    } else { \
        log_assert_fail(#expr, "", __FILE__, __LINE__); \
    } \
}

#define ASSERT_MSG(expr, msg) { \
    if (expr) { \
    } else { \
        log_assert_fail(#expr, msg, __FILE__, __LINE__); \
    } \
}
