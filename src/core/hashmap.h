#pragma once

#include "core/defines.h"
#include "core/memory.h"

typedef struct hashmap_entry_t {
    char* key;
    void* value;
    b8 is_occupied;
} hashmap_entry_t;

typedef struct hashmap_t {
    allocator_t* allocator;
    u64 length; 
    u64 capacity;
    u64 item_size;
    hashmap_entry_t* table;
} hashmap_t;

#define HASHMAP_DEFAULT_CAPACITY (64)

hashmap_t *hashmap_create(allocator_t* allocator, u64 item_size);

b8 hashmap_put(hashmap_t* hashmap, const char* key, void* value);

b8 hashmap_get(hashmap_t* hashmap, const char* key, void** dest);

b8 hashmap_remove(hashmap_t* hashmap, const char* key);

void hashmap_destroy(hashmap_t* hashmap);
