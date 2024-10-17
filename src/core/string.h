#pragma once

#include "core/defines.h"
#include "core/memory.h"
#include "core/str.h"

#include <stdarg.h>

typedef struct string_header_t {
    u64 capacity;
    u64 length;
    allocator_t* allocator;
} string_header_t;

typedef char* string_t;

#define STRING_DEFAULT_CAPACITY (16)

#define STRING_RESIZE_FACTOR (2)

#define string_header(string) ((string_header_t*)(string) - 1)

#define string_length(string) (((string_header_t*)(string) - 1)->length)

string_t string_create(allocator_t* allocator, const char* cstr);

string_t string_duplicate(allocator_t* allocator, string_t other);

string_t string_create_with_capacity(allocator_t* allocator, u64 capacity);

string_t string_create_with_length(allocator_t* allocator, const void* init_string, u64 length);

void string_append_str(string_t string, str_t str);

void string_append_string(string_t string, string_t other);

void string_append_cstr(string_t string, const char* cstr);

void string_append_format(string_t string, const char* format, ...);

void string_append_format_v(string_t string, const char* format, va_list args);

void string_append_length(string_t string, const char* other, u64 other_length);

void string_destroy(string_t string);

string_t string_maybe_resize(string_t string, u64 length_to_add);

b8 string_compare(string_t a, string_t b);

str_t strint_to_str(string_t string);
