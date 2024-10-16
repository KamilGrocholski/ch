#pragma once

#include "defines.h"
#include "core/memory.h"
#include "core/str.h"
#include <stdarg.h>

typedef struct allocator_t allocator_t;
typedef struct str_t str_t;

typedef struct string_t {
    u64 capacity;
    u64 length;
    const char* data;
    allocator_t* allocator;
} string_t;

#define STRING_NULL (string_t){0}

#define STRING_DEFAULT_CAPACITY (16)

#define STRING_RESIZE_FACTOR (2)

string_t string_create(allocator_t* allocator);

string_t string_create_with_capacity(allocator_t* allocator, u64 capacity);

string_t string_from_str(allocator_t* allocator, str_t str);

string_t string_from_cstr(allocator_t* allocator, const char* cstr);

string_t string_duplicate(allocator_t* allocator, string_t other);

void string_destroy(string_t* string);

void string_clear(string_t* string);

void string_append_string(string_t* string, string_t other);

void string_append_char(string_t* string, char ch);

void string_append_str(string_t* string, str_t str);

void string_append_cstr(string_t* string, const char* cstr);

void string_appendf(string_t* string, const char* format, ...);

void string_appendf_v(string_t* string, const char* format, va_list va_list_origin);

str_t string_to_str(string_t string);

b8 string_is_empty(string_t string);

b8 string_is_null(string_t string);

string_t _string_create(u64 capacity, allocator_t* allocator);

void _string_maybe_resize(string_t* string, u64 min_capacity);
