#include "core/arena.h"

#include "core/defines.h"
#include "core/logger.h"
#include "core/memory.h"

allocator_t arena_to_allocator(arena_t* arena) {
    if (!arena) {
        LOG_ERROR("arena_to_allocator - provided arena is 0.");
        return (allocator_t){0};
    }
    return (allocator_t){
        .allocate = arena_allocator_allocate,
        .free = arena_allocator_free,
        .free_all = arena_allocator_free_all,
        .context = arena,
    };
}

void arena_create(u64 total_size, void* memory, arena_t* arena) {
    if (!arena) {
        LOG_ERROR("arena_create - provided arena is 0.");
        return;
    }

    arena->total_size = total_size;
    arena->allocated = 0;
    arena->owns_memory = memory == 0;
    if (memory) {
        arena->memory = memory;
    } else {
        arena->memory = memory_allocate(total_size, MEMORY_TAG_ARENA);
    }
}

void arena_destroy(arena_t* arena) {
    if (!arena) {
        LOG_ERROR("arena_destroy - provided arena is 0.");
        return;
    }

    arena->allocated = 0;
    if (arena->owns_memory && arena->memory) {
        memory_free(arena->memory, arena->total_size, MEMORY_TAG_ARENA);
    }
    arena->memory = 0;
    arena->total_size = 0;
    arena->owns_memory = false;
}

void* arena_allocator_allocate(void* context, u64 size) {
    arena_t* arena = (arena_t*)context;

    if (!arena || !arena->memory) {
        LOG_ERROR("arena_allocator_allocate - provided allocator not initialized.");
        return 0;
    }

    if (arena->allocated + size > arena->total_size) {
        u64 remaining = arena->total_size - arena->allocated;
        LOG_ERROR("arena_allocator_allocate - trying to allocate %lluB, only %lluB remaining.", size, remaining);
        return 0;
    }
    void* block = ((u8*)arena->memory) + arena->allocated;
    arena->allocated += size;
    return block;
}

void arena_allocator_free(void* context, void* block, u64 size) {
    // The arena should not free it's memory partially
    // Keep it empty
}

void arena_allocator_free_all(void* context, b8 should_clear) {
    arena_t* arena = (arena_t*)context;
    if (!arena || !arena->memory) {
        LOG_ERROR("arena_allocator_free_all - provided allocator not initialized.");
        return;
    }

    arena->allocated = 0;
    if (should_clear) {
        memory_zero(arena->memory, arena->total_size);
    }
}
