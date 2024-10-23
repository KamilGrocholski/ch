#pragma once

#include "core/defines.h"
#include "core/memory.h"
#include "core/str.h"

#define STRHASHMAP_MIN_CAPACITY (32)
#define STRHASHMAP_GROW_FACTOR (2)
#define STRHASHMAP_CAPACITY_THRESHHOLD (0.75f)

typedef struct strhashmap_entry_t {
    str_t key;
    str_t value;
    b8 is_occupied;
} strhashmap_entry_t;

typedef struct strhashmap_t {
    strhashmap_entry_t* table;
    u64 capacity;
    u64 length;
    allocator_t* allocator;
} strhashmap_t;

void strhashmap_init(allocator_t* allocator, strhashmap_t* strhashmap);

void strhashmap_deinit(strhashmap_t* strhashmap);

b8 strhashmap_set(strhashmap_t* strhashmap, str_t key, str_t value);

b8 strhashmap_get(strhashmap_t* strhashmap, str_t key, str_t* value);

b8 strhashmap_remove(strhashmap_t* strhashmap, str_t key);
