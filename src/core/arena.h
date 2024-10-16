#pragma once

#include "defines.h"
#include "core/memory.h"

typedef struct arena_t {
    u64 total_size;
    u64 allocated;
    void* memory;
    b8 owns_memory;
} arena_t;

allocator_t arena_to_allocator(arena_t* arena);

void arena_create(u64 total_size, void* memory, arena_t* out_arena);

void arena_destroy(arena_t* arena);

void* arena_allocator_allocate(void* context, u64 size);

void arena_allocator_free(void* context, void* block, u64 size);

void arena_allocator_free_all(void* context, b8 should_clear);
