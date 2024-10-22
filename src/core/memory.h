#pragma once

#include "core/defines.h"

typedef enum memory_tag_t {
    MEMORY_TAG_UNKNOWN = 0,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_STRING,
    MEMORY_TAG_ARENA,
    MEMORY_TAG_TRIE_NODE,
    MEMORY_TAG_HASHMAP,
    MEMORY_TAG_HTTP_HEADERS,
    MEMORY_TAG_MAX_TAGS,
} memory_tag_t;

typedef struct allocator_t {
    void* (*allocate)(void* context, u64 size);
    void (*free)(void* context, void* block, u64 size);
    void (*free_all)(void* context, b8 should_clear);
    void* context;
} allocator_t;

void memory_init(void);

void memory_shutdown(void);

void* memory_allocate(u64 size, memory_tag_t tag);

void* memory_reallocate(void* block, u64 old_size, u64 new_size, memory_tag_t tag);

void memory_free(void* block, u64 size, memory_tag_t tag);

void* memory_copy(void* dest, const void* src, u64 size);

void* memory_set(void* dest, i32 value, u64 size);

void* memory_zero(void* dest, u64 size);

char* memory_stats_to_string(void);

void memory_report(void);
