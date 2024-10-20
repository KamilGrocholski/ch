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

u64 string_length(string_t string);

void string_length_set(string_t string, u64 length);

string_t string_create(allocator_t *allocator, const char* cstr);

string_t string_from_parts(allocator_t *allocator, const void* init, u64 length);

string_t string_duplicate(string_t string);

string_t string_append_cstr(string_t string, const char* cstr);

string_t string_append_format(string_t string, const char* format, ...);

string_t string_append_format_v(string_t string, const char* format, va_list args);

void string_destroy(string_t string);

str_t string_to_str(string_t string);
