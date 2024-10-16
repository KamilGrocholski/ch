#include "core/string.h"

#include "core/defines.h"
#include "core/logger.h"
#include "core/str.h"
#include "core/memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

string_t string_create(allocator_t* allocator) {
    return _string_create(STRING_DEFAULT_CAPACITY, allocator);
}

string_t string_create_with_capacity(allocator_t* allocator, u64 capacity) {
    return _string_create(capacity, allocator);
}

string_t string_from_str(allocator_t* allocator, str_t str) {
    string_t string = _string_create(str.length, allocator);
    memory_copy((char*)string.data, str.data, str.length);
    return string;
}

string_t string_from_cstr(allocator_t* allocator, const char* cstr) {
    u64 length = strlen(cstr);
    string_t string = _string_create(length, allocator);
    memory_copy((char*)string.data, cstr, length);
    string.length = length;
    return string;
}

string_t string_duplicate(allocator_t* allocator, string_t other) {
    string_t string = _string_create(other.capacity, allocator);
    memory_copy((void*)string.data, (void*)other.data, other.length);
    string.length = other.length;
    return string;
}

void string_destroy(string_t* string) {
    if (string->allocator) {
        string->allocator->free(string->allocator->context, (void*)string->data, string->capacity);
    } else {
        memory_free((void*)string->data, string->capacity, MEMORY_TAG_STRING);
    }
    string->capacity = 0;
    string->length = 0;
    string->allocator = 0;
    string->data = 0;
}

void string_clear(string_t* string) {
    string->length = 0;
}

void string_append_string(string_t* string, string_t other) {
    _string_maybe_resize(string, string->length + other.length);
    memory_copy((void*)string->data + string->length, (void*)other.data, other.length);
    string->length += other.length;
}

void string_append_char(string_t* string, char ch) {
    _string_maybe_resize(string, string->length + 1);
    ((char*)string->data)[string->length++] = ch;
}

void string_append_str(string_t* string, str_t str) {
    _string_maybe_resize(string, string->length + str.length);
    memory_copy((void*)string->data + string->length, (void*)str.data, str.length);
    string->length += str.length;
}

void string_append_cstr(string_t* string, const char* cstr) {
    str_t str = str_from_cstr(cstr);
    _string_maybe_resize(string, string->length + str.length);
    memory_copy((void*)string->data + string->length, (void*)str.data, str.length);
    string->length += str.length;
}

void string_appendf(string_t* string, const char* format, ...) {
    if (!format) {
        return;
    }
	va_list args;
	va_start(args, format);
    string_appendf_v(string, format, args);
	va_end(args);
}

void string_appendf_v(string_t* string, const char* format, va_list va_list_origin) {
    if (!format) {
        return;
    }
    i32 length = 0;
    va_list va_list_copy;
    va_copy(va_list_copy, va_list_origin);
    length = vsnprintf(0, 0, format, va_list_copy);
    va_end(va_list_copy);

    if (length < 0) {
        return;
    }
    _string_maybe_resize(string, string->length + length + 1);
    vsnprintf(&((char*)string->data)[string->length], length + 1, format, va_list_origin);
    string->length += length;
}

str_t string_to_str(string_t string) {
    return (str_t){
        .length = string.length,
        .data = string.data,
    };
}

b8 string_is_empty(string_t string) {
    return string.length != 0;
}

b8 string_is_null(string_t string) {
    return string.data == 0;
}

string_t _string_create(u64 capacity, allocator_t* allocator) {
    string_t string = (string_t){
        .data = 0,
        .capacity = capacity,
        .length = 0,
        .allocator = allocator,
    };
    if (allocator) {
        string.data = allocator->allocate(string.allocator->context, capacity);
    } else {
        string.data = memory_allocate(capacity, MEMORY_TAG_STRING);
    }
    if (!string.data) {
        string.data = 0;
        string.capacity = 0;
    }
    return string;
}

void _string_maybe_resize(string_t* string, u64 min_capacity) {
    if (string->capacity >= min_capacity) {
        return;
    }
    u64 new_capacity = string->capacity * STRING_RESIZE_FACTOR;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    string_t new_string = _string_create(new_capacity, string->allocator);
    memory_copy((void*)new_string.data, (void*)string->data, string->length);
    new_string.length = string->length;
    string_destroy(string);
    *string = new_string;
}
