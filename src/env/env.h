#pragma once

#include "core/defines.h"
#include "core/memory.h"
#include "core/str.h"
#include "core/strhashmap.h"

typedef str_t (*env_getter_t)(const char* key);

typedef struct env_t {
    u64 PORT;
} env_t;

b8 env_load(allocator_t* allocator, const char* path, env_t *dest);

b8 env_parse_file(allocator_t* allocator, const char* path, strhashmap_t* dest);
