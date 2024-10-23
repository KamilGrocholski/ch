#pragma once

#include <core/logger.h>
#include <core/str.h>

#include <string.h>

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

#define expect_str_eq_cstr(str, cstr) do { \
    u32 cstr_length = strlen(cstr); \
    expect_neq(0, str.data); \
    expect_eq(cstr_length, cstr_length); \
    for (u32 i = 0; i < cstr_length; i++) { \
        expect_eq(cstr[i], str.data[i]); \
    } \
} while(0);
