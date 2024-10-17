#pragma once

#include "core/defines.h"
#include "core/memory.h"

typedef struct array_header_t {
    u64 capacity;
    u64 length;
    u64 stride;
    allocator_t* allocator;
} array_header_t;

#define ARRAY_DEFAULT_CAPACITY (1)

#define ARRAY_RESIZE_FACTOR (2)

#define array_header(array) ((array_header_t*)(array) - 1)

#define array_length(array) ((array_header_t*)(array) - 1)->length

#define array_create(allocator, type) (type*)_array_create(sizeof(type), ARRAY_DEFAULT_CAPACITY, allocator)

#define array_append(array, item) do { \
    (array) = array_maybe_resize(array, array_length(array) + 1); \
    (array)[array_length(array)++] = (item); \
} while(0);

void array_destroy(void* array);

void array_clear(void* array);

void* _array_create(u64 stride, u64 capacity, allocator_t* allocator);

void* array_maybe_resize(void* array, u64 min_capacity);
