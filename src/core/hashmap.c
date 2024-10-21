#include "core/hashmap.h"

#include "core/memory.h"

#include <string.h>
#include <stdio.h>

static u64 _hash(const char* key) {
    u64 sum = 0;
    while (*key) {
        sum += (u64)*key++;
    }
    return sum;
}

static b8 _key_compare(const char* a, u64 a_length, const char* b) {
    return (strncmp(a, b, a_length) == 0);
}

static hashmap_t* _allocate(allocator_t* allocator, u64 item_size, u64 capacity) {
    hashmap_t* hashmap = 0;
    hashmap_entry_t* table = 0;
    u64 size = sizeof(hashmap_t);
    u64 table_size = capacity * sizeof(hashmap_entry_t);
    if (allocator) {
        hashmap = allocator->allocate(allocator->context, size);
        table = allocator->allocate(allocator->context, table_size);
    } else {
        hashmap = memory_allocate(size, MEMORY_TAG_HASHMAP);
        table = memory_allocate(table_size, MEMORY_TAG_HASHMAP);
    }
    if (!hashmap || !table) {
        return 0;
    };
    hashmap->table = table;
    hashmap->item_size = item_size;
    hashmap->capacity = capacity;
    hashmap->length = 0;
    hashmap->allocator = allocator;
    return hashmap;
}

static void _resize(hashmap_t* hashmap, u64 new_capacity) {
    if (!hashmap) {
        return;
    }
    hashmap_entry_t* old_table = hashmap->table;
    hashmap_entry_t* new_table = 0;
    u64 table_size = new_capacity * sizeof(hashmap_entry_t);
    u64 old_table_size = hashmap->capacity * sizeof(hashmap_entry_t);
    if (hashmap->allocator) {
        new_table = hashmap->allocator->allocate(hashmap->allocator->context, table_size);
    } else {
        new_table = memory_allocate(table_size, MEMORY_TAG_HASHMAP);
    }
    if (!new_table) {
        return;
    };
    hashmap->table = new_table;
    for (u64 i = 0; i < hashmap->capacity; i++) {
        hashmap_entry_t* entry = &(old_table[i]);
        if (entry->is_occupied) {
            hashmap_put(hashmap, entry->key, entry->value);
        }
    }
    if (hashmap->allocator) {
        hashmap->allocator->free(hashmap->allocator->context, old_table, old_table_size);
    } else {
        memory_free(old_table, old_table_size, MEMORY_TAG_HASHMAP);
    }
}

hashmap_t* hashmap_create(allocator_t* allocator, u64 item_size) {
    return _allocate(allocator, item_size, HASHMAP_DEFAULT_CAPACITY);
}

b8 hashmap_put(hashmap_t* hashmap, const char* key, void* value) {
    if ((f64)hashmap->length > (f64)(hashmap->capacity * 0.75f)) {
        _resize(hashmap, hashmap->capacity * 2);
    }
    u64 hash = _hash(key);
    u64 index = hash % hashmap->capacity;
    hashmap_entry_t* entry = &(hashmap->table[index]);
    u32 key_length = strlen(key);
    while (entry->is_occupied && !_key_compare(key, key_length, entry->key)) {
        index = (index + 1) % hashmap->capacity;
        entry = &(hashmap->table[index]);
    }
    if (entry->is_occupied && _key_compare(key, key_length, entry->key)) {
        memory_copy(entry->value, value, hashmap->item_size);
        return;
    };
    if (hashmap->allocator) {
        entry->key = hashmap->allocator->allocate(hashmap->allocator->context, key_length + 1);
        entry->value = hashmap->allocator->allocate(hashmap->allocator->context, hashmap->item_size);
    } else {
        entry->key = memory_allocate(key_length + 1, MEMORY_TAG_HASHMAP);
        entry->value = memory_allocate(hashmap->item_size, MEMORY_TAG_HASHMAP);
    }
    if (!entry->key || !entry->value) {
        return false;
    }
    entry->is_occupied = true;
    memory_copy(entry->key, key, key_length + 1);
    memory_copy(entry->value, value, hashmap->item_size);
    hashmap->length++;
    return true;
}

b8 hashmap_get(hashmap_t* hashmap, const char* key, void** dest) {
    u64 hash = _hash(key);
    u64 index = hash % hashmap->capacity;
    hashmap_entry_t* entry = &(hashmap->table[index]);
    u32 key_length = strlen(key);
    while (entry->is_occupied) {
        if (_key_compare(key, key_length, entry->key)) {
            *dest = entry->value;
            return true;
        }
        index = (index + 1) % hashmap->capacity;
        entry = &(hashmap->table[index]);
    }
    return false;
}

b8 hashmap_remove(hashmap_t* hashmap, const char* key) {
    if (!hashmap->length) {
        return false;
    }
    u64 hash = _hash(key);
    u64 index = hash % hashmap->capacity;
    hashmap_entry_t* entry = &(hashmap->table[index]);
    u32 key_length = strlen(key);
    while (entry->is_occupied) {
        if (_key_compare(key, key_length, entry->key)) {
            entry->is_occupied = false;
            hashmap->length--;
            return true;
        }
        index = (index + 1) % hashmap->capacity;
        entry = &(hashmap->table[index]);
    }
    return false;
}

void hashmap_destroy(hashmap_t* hashmap) {
    if (!hashmap) {
        return;
    }

    u64 table_size = hashmap->capacity * sizeof(hashmap_entry_t);

    for (u64 i = 0; i < hashmap->capacity; ++i) {
        hashmap_entry_t* entry = &(hashmap->table[i]);
        if (entry->is_occupied) {
            if (hashmap->allocator) {
                hashmap->allocator->free(hashmap->allocator->context, entry->key, strlen(entry->key) + 1);
                hashmap->allocator->free(hashmap->allocator->context, entry->value, hashmap->item_size);
            } else {
                memory_free(entry->key, strlen(entry->key) + 1, MEMORY_TAG_HASHMAP);
                memory_free(entry->value, hashmap->item_size, MEMORY_TAG_HASHMAP);
            }
        }
    }

    if (hashmap->allocator) {
        hashmap->allocator->free(hashmap->allocator->context, hashmap->table, table_size);
        hashmap->allocator->free(hashmap->allocator->context, hashmap, sizeof(hashmap_t));
    } else {
        memory_free(hashmap->table, table_size, MEMORY_TAG_HASHMAP);
        memory_free(hashmap, sizeof(hashmap_t), MEMORY_TAG_HASHMAP);
    }
}
