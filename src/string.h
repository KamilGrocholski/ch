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

string_t string_create_with_capacity(allocator_t* allocator, u64 capacity);

string_t string_create_with_length(allocator_t* allocator, const void* init_string, u64 length);

void string_destroy(string_t* string);
