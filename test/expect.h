#pragma once

#include <core/logger.h>

#define expect_eq(expected, actual) \
    if (actual != expected) { \
        LOG_ERROR("--> Expected %lld, but got: %lld. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return false; \
    }

#define expect_neq(expected, actual) \
    if (actual == expected) { \
        LOG_ERROR("--> Expected %d != %d, but they are equal. File: %s:%d.", expected, actual, __FILE__, __LINE__); \
        return false; \
    }

#define expect_true(actual) \
    if (actual != true) { \
        LOG_ERROR("--> Expected true, but got: false. File: %s:%d.", __FILE__, __LINE__); \
        return false; \
    }

#define expect_false(actual) \
    if (actual != false) { \
        LOG_ERROR("--> Expected false, but got: true. File: %s:%d.", __FILE__, __LINE__); \
        return false; \
    }
