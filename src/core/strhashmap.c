#include "core/strhashmap.h"

#include "core/logger.h"
#include "core/assert.h"
#include "core/defines.h"

static u64 _hash(str_t key, u64 capacity) {
    u64 hash = 0;
    for (u64 i = 0; i < key.length; i++) {
        hash = (hash * 31) + key.data[i];
    }
    return hash % capacity;
}

static void strhashmap_resize(strhashmap_t* strhashmap, u64 new_capacity) {
    if (new_capacity < STRHASHMAP_MIN_CAPACITY) {
        new_capacity = STRHASHMAP_MIN_CAPACITY;
    }
    strhashmap_entry_t* old_table = strhashmap->table;
    strhashmap_entry_t* new_table = 0;
    u64 old_capacity = strhashmap->capacity;
    strhashmap->capacity = new_capacity;
    strhashmap->length = 0;
    u64 new_size = new_capacity * sizeof(strhashmap_entry_t);
    u64 old_size = old_capacity * sizeof(strhashmap_entry_t);
    if (strhashmap->allocator) {
        new_table = strhashmap->allocator->allocate(strhashmap->allocator->context, new_size);
    } else {
        new_table = memory_allocate(new_size, MEMORY_TAG_STRHASHMAP);
    }
    if (!new_table) {
        return;
    }
    strhashmap->table = new_table;
    memory_zero(strhashmap->table, new_size);
    for (u64 i = 0; i < old_capacity; i++) {
        if (old_table[i].is_occupied) {
            strhashmap_set(strhashmap, old_table[i].key, old_table[i].value);
        }
    }
    if (strhashmap->allocator) {
        strhashmap->allocator->free(strhashmap->allocator->context, old_table, old_size);
    } else {
        memory_free(old_table, old_size, MEMORY_TAG_STRHASHMAP);
    }
}

void strhashmap_init(allocator_t* allocator, strhashmap_t* strhashmap) {
    if (!strhashmap) {
        LOG_ERROR("strhashmap_init - called with strhashmap 0.");
        return;
    }
    strhashmap->allocator = allocator;
    strhashmap->capacity = 64;
    strhashmap->length = 0;
    u64 size = sizeof(strhashmap_entry_t) * strhashmap->capacity;
    if (allocator) {
        strhashmap->table = allocator->allocate(allocator->context, size);
    } else {
        strhashmap->table = memory_allocate(size, MEMORY_TAG_STRHASHMAP);
    }
    if (!strhashmap->table) {
        LOG_ERROR("strhashmap_init - could not allocate table memory.");
        return;
    }
    memory_zero(strhashmap->table, size);
}

b8 strhashmap_set(strhashmap_t* strhashmap, str_t key, str_t value) {
    if (str_is_null(key) || str_is_null(value)) {
        return false;
    }
    if (str_is_empty(key)) {
        return false;
    }
    if (strhashmap->length >= (u64)(strhashmap->capacity * STRHASHMAP_CAPACITY_THRESHHOLD)) {
        strhashmap_resize(strhashmap, strhashmap->capacity * STRHASHMAP_GROW_FACTOR);
    }    
    u64 index = _hash(key, strhashmap->capacity);
    u64 original_index = index;
    strhashmap_entry_t* entry = &(strhashmap->table[index]);
    while (entry->is_occupied && !str_compare(entry->key, key)) {
        index = (index + 1) % strhashmap->capacity;
        if (index == original_index) {
            return false;
        }
        entry = &(strhashmap->table[index]);
    }
    if (entry->is_occupied && str_compare(entry->key, key)) {
        if (value.length > entry->value.length) {
            if (strhashmap->allocator) {
                const char* new_data = strhashmap->allocator->allocate(strhashmap->allocator->context, value.length);
                if (!new_data) {
                    return false;
                }
                strhashmap->allocator->free(strhashmap->allocator->context, (void*)value.data, value.length);
                entry->value.data = new_data;
            } else {
                const char* new_data = memory_allocate(value.length, MEMORY_TAG_STRHASHMAP);
                if (!new_data) {
                    return false;
                }
                memory_free((void*)value.data, value.length, MEMORY_TAG_STRHASHMAP);
                entry->value.data = new_data;
            }
        }
        memory_copy((void*)entry->value.data, value.data, value.length);
        entry->value.length = value.length;
        return true;
    };
    if (strhashmap->allocator) {
        entry->key.data = strhashmap->allocator->allocate(strhashmap->allocator->context, key.length);
        entry->value.data = strhashmap->allocator->allocate(strhashmap->allocator->context, value.length);
    } else {
        entry->key.data = memory_allocate(key.length, MEMORY_TAG_STRHASHMAP);
        entry->value.data = memory_allocate(value.length, MEMORY_TAG_STRHASHMAP);
    }
    if (!entry->key.data || !entry->value.data) {
        return false;
    }
    memory_copy((void*)entry->key.data, key.data, key.length);
    entry->key.length = key.length;
    memory_copy((void*)entry->value.data, value.data, value.length);
    entry->value.length = value.length;
    entry->is_occupied = true;
    strhashmap->length++;
    return true;
}

static b8 _strhashmap_get(strhashmap_t* strhashmap, str_t key, str_t* value, b8 ci) {
    if (!strhashmap || !strhashmap->table) return false;
    u64 index = _hash(key, strhashmap->capacity);
    u64 original_index = index;
    strhashmap_entry_t* entry = &(strhashmap->table[index]);
    while (entry->is_occupied) {
        b8 eq;
        if (ci) {
            eq = str_compare_ci(entry->key, key);
        } else {
            eq = str_compare(entry->key, key);
        }
        if (eq) {
            *value = entry->value;
            return true;
        }
        index = (index + 1) % strhashmap->capacity;
        if (index == original_index) return false;  // Full circle, key not found
        entry = &(strhashmap->table[index]);
    }
    return false;
}

b8 strhashmap_get(strhashmap_t* strhashmap, str_t key, str_t* value) {
    return _strhashmap_get(strhashmap, key, value, false);
}

b8 strhashmap_get_ci(strhashmap_t* strhashmap, str_t key, str_t* value) {
    return _strhashmap_get(strhashmap, key, value, true);
}

b8 strhashmap_remove(strhashmap_t* strhashmap, str_t key) {
    if (!strhashmap || !strhashmap->table) return false;
    u64 index = _hash(key, strhashmap->capacity);
    u64 original_index = index;

    while (strhashmap->table[index].is_occupied) {
        if (str_compare(strhashmap->table[index].key, key)) {
            strhashmap->table[index].is_occupied = false;
            strhashmap->length--;
            return true;
        }
        index = (index + 1) % strhashmap->capacity;
        if (index == original_index) break;  // Full circle, key not found
    }

    return false;
}

void strhashmap_deinit(strhashmap_t* strhashmap) {
    if (!strhashmap || !strhashmap->table) return;
    for (u64 i = 0; i < strhashmap->capacity; i++) {
        strhashmap_entry_t* e = &(strhashmap->table[i]);
        if (strhashmap->table[i].is_occupied) {
            if (strhashmap->allocator) {
                strhashmap->allocator->free(strhashmap->allocator->context, (void*)e->key.data, e->key.length);
                strhashmap->allocator->free(strhashmap->allocator->context, (void*)e->value.data, e->value.length);
            } else {
                memory_free((void*)e->key.data, e->key.length, MEMORY_TAG_STRHASHMAP);
                memory_free((void*)e->value.data, e->value.length, MEMORY_TAG_STRHASHMAP);
            }
        }
    }
    if (strhashmap->allocator) {
        strhashmap->allocator->free(strhashmap->allocator->context, strhashmap->table, strhashmap->capacity * sizeof(strhashmap_entry_t));
    } else {
        memory_free(strhashmap->table, strhashmap->capacity * sizeof(strhashmap_entry_t), MEMORY_TAG_STRHASHMAP);
    }
    strhashmap->table = 0;
    strhashmap->length = 0;
    strhashmap->capacity = 0;
}

string_t strhashmap_to_string(allocator_t* allocator, strhashmap_t* strhashmap, const char* kv_format) {
    if (!kv_format) {
        kv_format = "[%.*s]: '%.*s'";
    }
    string_t string = string_from_parts(allocator, 0, strhashmap->length * 32);
    if (strhashmap->length == 0) {
        return string;
    }
    u64 i = 0;
    strhashmap_entry_t* curr = 0;
    do {
        curr = &(strhashmap->table[i++]);
        if (curr->is_occupied && string) {
            string = string_append_format(string, kv_format, curr->key.length, curr->key.data, curr->value.length, curr->value.data);
        }
    } while(i < strhashmap->capacity);
    return string;
}
