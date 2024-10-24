#include "core/string.h"

#include "core/defines.h"
#include "core/memory.h"
#include "core/logger.h"

#include <string.h>
#include <stdarg.h>
#include <assert.h>

static string_header_t* _header_allocate(allocator_t* allocator, u64 length);
static string_t _string_from_parts(allocator_t* allocator, const void* init, u64 length);
static string_t _string_maybe_grow(string_t string, u64 wanted_length);
static string_t _string_append_length(string_t string, const void* ptr, u64 length);

u64 string_length(string_t string) {
    return string_header(string)->length;
}

void string_length_set(string_t string, u64 length) {
    string_header_t* header = string_header(string);
    header->length = length;
}

string_t string_create(allocator_t *allocator, const char* cstr) {
    return _string_from_parts(allocator, cstr, strlen(cstr));
}

string_t string_from_parts(allocator_t *allocator, const void* init, u64 length) {
    return _string_from_parts(allocator, init, length);
}

string_t string_from_str(allocator_t *allocator, str_t str) {
    return _string_from_parts(allocator, str.data, str.length);
}

string_t string_duplicate(string_t string) {
    string_header_t* header = string_header(string);
    return _string_from_parts(header->allocator, string, header->length);
}

string_t string_append_string(string_t string, string_t other) {
    return _string_append_length(string, other, string_length(other));
}

string_t string_append_cstr(string_t string, const char* cstr) {
    return _string_append_length(string, cstr, strlen(cstr));
}

string_t string_append_format(string_t string, const char* format, ...) {
    if (!format) {
        return string;
    }
    va_list args;
    va_start(args, format);
    string = string_append_format_v(string, format, args);
    va_end(args);
    return string;
}

string_t string_append_format_v(string_t string, const char* format, va_list args) {
    if (!format) {
        return string;
    }
    va_list args_copy;
    va_copy(args_copy, args);
    i32 length_check = vsnprintf(0, 0, format, args_copy);
    va_end(args_copy);
    if (length_check < 0) {
        return string;
    }
    u32 length = (u32)length_check;
    u64 new_length = string_length(string) + length;
    string = _string_maybe_grow(string, new_length);
    vsnprintf(string + string_length(string), length + 1, format, args);
    string_length_set(string, new_length);
    string[new_length] = '\0';
    return string;
}

void string_destroy(string_t string) {
    if (!string) {
        LOG_ERROR("string_destroy - called with string 0.");
        return;
    }
    string_header_t* header = string_header(string);
    u64 size = sizeof(string_header_t) + header->capacity;
    if (header->allocator) {
        header->allocator->free(header->allocator->context, header, size);
    } else {
        memory_free(header, size, MEMORY_TAG_STRING);
    }
}

str_t string_to_str(string_t string) {
    if (!string) {
        return STR_NULL;
    }
    return (str_t){.length = string_length(string), .data = string};
}

static string_header_t* _header_allocate(allocator_t* allocator, u64 length) {
    string_header_t* header = 0;
    u64 capacity = length + 1;
    u64 size = capacity + sizeof(string_header_t);
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
    return header;
}

static string_t _string_from_parts(allocator_t* allocator, const void* init, u64 length) {
    string_header_t* header = _header_allocate(allocator, length);
    if (!header) {
        return 0;
    }
    string_t string = (string_t)(header + 1);
    if (init && length) {
        memory_copy(string, init, length);
        header->length = length;
    }
    string[length] = '\0';
    return string;
}

static string_t _string_maybe_grow(string_t string, u64 wanted_length) {
    string_header_t* header = string_header(string);
    if (header->capacity > wanted_length) {
        return string;
    }
    string_header_t* new_header = _header_allocate(header->allocator, wanted_length);
    if (!new_header) {
        return 0;
    }
    string_t new_string = (string_t)(new_header + 1);
    memory_copy(new_string, string, header->length);
    string_length_set(new_string, header->length);
    new_string[header->length] = '\0';
    string_destroy(string);
    return new_string;
}

static string_t _string_append_length(string_t string, const void* ptr, u64 length) {
    if (!ptr || !length) {
        return string;
    }
    u64 old_length = string_length(string);
    u64 new_length = old_length + length;
    string_t out = _string_maybe_grow(string, new_length);
    memory_copy(out + old_length, ptr, length);
    string_length_set(out, new_length);
    out[new_length] = '\0';
    return out;
}
