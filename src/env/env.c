#include "env/env.h"

#include "core/logger.h"
#include "fs/fs.h"

b8 env_load(allocator_t* allocator, const char* path, env_t *dest) {
    strhashmap_t env = {0};
    strhashmap_init(allocator, &env); 

    if (!env_parse_file(allocator, path, &env)) {
        return false;
    }

    // TODO create functions for reading with defaults and parsing envs (e.g. env_u64, env_str, ...)
    str_t port;    
    if (!strhashmap_get(&env, str_from_cstr("PORT"), &port)) {
        return false;
    } else {
        if (str_is_empty(port)) {
            dest->PORT = 8080;
        } else {
            if (!str_to_u64(port, &dest->PORT)) {
                return false;
            }
        }
    }
    
    return true;
}

b8 env_parse_file(allocator_t* allocator, const char* path, strhashmap_t* dest) {
    string_t content = fs_read_entire_text(allocator, path);
    if (!content) {
        LOG_ERROR("env_parse_file - could not load env file");
        return false;
    }

    str_t rest = string_to_str(content);
    while(!str_is_empty(rest)) {
        str_t line = str_pop_first_split_char(&rest, '\n');
        str_t key = str_pop_first_split_char(&line, '=');
        str_t value = line;
        LOG_DEBUG("env_parse_file - kv [%.*s]: '%.*s'", key.length, key.data, value.length, value.data);
        if (!strhashmap_set(dest, key, value)) {
            return false;
        }
    }
    
    return true;
}
