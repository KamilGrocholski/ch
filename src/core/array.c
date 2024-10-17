#include "core/array.h"

#include "core/memory.h"
#include "core/logger.h"

void array_destroy(void* array) {
    if (!array) {
        LOG_ERROR("array_destroy - called with array 0.");
        return;
    }
    array_header_t* header = array_header(array);
    u64 size = sizeof(array_header_t) + header->stride * header->capacity;
    if (header->allocator) {
        header->allocator->free(header->allocator->context, header, size);
    } else {
        memory_free(header, size, MEMORY_TAG_ARRAY);
    }
    array = 0;
}

void array_clear(void* array) {
    array_length(array) = 0;
}

void* _array_create(u64 stride, u64 capacity, allocator_t* allocator) {
    array_header_t* header = 0;
    u64 size = sizeof(array_header_t) + stride * capacity;
    if (allocator) {
        header = allocator->allocate(header->allocator->context, size);
    } else {
        header = memory_allocate(size, MEMORY_TAG_ARRAY);
    }
    if (!header) {
        return 0;
    }
    header->length = 0;
    header->capacity = capacity;
    header->stride = stride;
    header->allocator = allocator;
    return header + 1;
}

void* array_maybe_resize(void* array, u64 min_capacity) {
    array_header_t* old_header = array_header(array);
    if (old_header->capacity >= min_capacity) return array;
    u64 header_size = sizeof(array_header_t);
    u64 old_size = header_size + old_header->length * old_header->stride;
    u64 new_capacity = old_header->capacity * ARRAY_RESIZE_FACTOR;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    void* new_array = _array_create(old_header->stride, new_capacity, old_header->allocator);
    array_header_t* new_header = array_header(new_array);
    memory_copy(new_header, old_header, old_size);
    new_header->capacity = new_capacity;
    array_destroy(array);
    return new_header + 1;
}
