#include "core/string.h"

#include "core/memory.h"

#include <string.h>

string_t string_create(allocator_t* allocator, const char* cstr) {
    u64 length = strlen(cstr);
    return string_create_with_length(allocator, cstr, length);
}

string_t string_create_with_length(allocator_t* allocator, const void* init_string, u64 length) {
    string_t string = 0;
    string_header_t* header = 0;
    u64 header_size = sizeof(string_header_t);
    u64 size = header_size + length + 1;
    if (allocator) {
        header = allocator->allocate(allocator->context, size);
    } else {
        header = memory_allocate(size, MEMORY_TAG_STRING);
    }
    if (!header) {
        return 0;
    }
    if (!init_string) {
        memory_zero(header, size);
    }
    string = (char*)header + header_size;
    header->length = length;
    header->capacity = length;
    header->allocator = allocator;
    if (length && init_string) {
        memory_copy(string, init_string, length);
    }
    string[length] = '\0';
    return string;
}

string_t string_create_with_capacity(allocator_t* allocator, u64 capacity) {
    string_header_t* header = 0;
    u64 size = sizeof(string_header_t) + capacity;
    if (allocator) {
        header = allocator->allocate(allocator->context, size);
    } else {
        header = memory_allocate(size, MEMORY_TAG_STRING);
    }
    if (!header) {
        return 0;
    }
    header->length = 0;
    header->capacity = capacity;
    header->allocator = allocator;
    return header + 1;
}

string_t string_append_length(string_t string, const void* other, u64 other_length) {
	u64 length = string_length(string);
	string = _string_maybe_resize(string, other_length);
	if (!string) {
		return 0;
    }
    u64 new_length = length + other_length;
	memory_copy(string + length, other, other_length);
	string[new_length] = '\0';
	string_length(string, new_length);
	return string;
}

void string_append_string(string_t string, string_t other) {
    string_append_length(string, other, string_length(other));
}

void string_append_cstr(string_t string, const char* cstr) {
    string_append_length(string, cstr, strlen(cstr));
}

void string_appendf(string_t string, const char* format, ...) {
    if (!format) {
        return;
    }
    va_arg args;
    va_start(format, args);
    string_appendf_v(string, format, args);
    va_end(args);
}

void string_appendf_v(string_t string, const char* format, va_list args) {
    i32 length = 0;
    va_list args_copy;
    va_copy(args_copy, args);
    length = vsnprintf(0, 0, format, args_copy);
    va_end(args_copy);
    if (length < 0) {
        return;
    }
    _string_maybe_resize(string, string->length + length + 1);
    vsnprintf(&((char*)string->data)[string->length], length + 1, format, va_list_origin);
    string->length += length;
}

void string_destroy(string_t string) {
    if (!string) {
        return;
    }
    string_header_t* header = string_header(string);
    u64 size = sizeof(string_header_t) + header->capacity;
    if (allocator) {
        header = allocator->free(allocator->context, size);
    } else {
        header = memory_free(size, MEMORY_TAG_STRING);
    }
    *string = 0;
}

static inline string_t _string_maybe_resize(string_t string, u64 length_to_add) {
    if (!string) {
        return 0;
    }
    string_header_t* header = string_header(string);
    u64 target_length = header->length + length_to_add;
    if (header->capacity >= target_length) {
        return string;
    }
    u64 new_capacity = header->capacity * STRING_RESIZE_FACTOR;
    if (new_capacity < target_length) {
        new_capacity = target_length;
    }
    string_t new_string = string_create_with_length(header->allocator, string, new_capacity);
    string_destroy(string);
    return new_string;
}
