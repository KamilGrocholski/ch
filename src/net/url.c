#include "net/url.h"

#include "core/str.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/defines.h"
#include "core/logger.h"
#include "core/assert.h"

// For now just simple cases, no RFC
// e.g. http://localhost:8080/users/2/friends?name=john&age=20#fragment
b8 url_parse(str_t raw_url, url_t* dest) {
    str_t base = str_pop_first_split_char(&raw_url, '?'); 
    dest->raw_query = str_pop_first_split_char(&raw_url, '#');
    dest->raw_fragment = raw_url;
    dest->scheme = str_pop_first_split(&base, str_from_cstr("://"));
    dest->host = str_pop_first_split_char(&base, ':');
    dest->port = str_pop_first_split_char(&base, '/');
    dest->path = base; // path without '/'

    return true;
}

void url_query_init(allocator_t* allocator, url_query_t* dest) {
    ASSERT(dest);
    strhashmap_init(allocator, &dest->values);
}

void url_query_deinit(url_query_t* query) {
    if (!query) {
        LOG_ERROR("url_query_deinit - called with query 0.");
        return;
    }
    strhashmap_deinit(&query->values);
}

b8 url_query_parse(str_t raw_query, url_query_t* dest) {
    ASSERT(dest);
    if (str_is_null(raw_query) || str_is_empty(raw_query)) {
        return false;
    }

    str_t key;
    str_t value;
    str_t pair;
    do {
        pair = str_pop_first_split_char(&raw_query, '&');
        key = str_pop_first_split_char(&pair, '=');
        value = pair;
        if (!url_query_set(dest, key, value)) {
            return false;
        }
    } while(!str_is_empty(raw_query));

    return true;
}

b8 url_query_set(url_query_t* query, str_t key, str_t value) {
    return strhashmap_set(&query->values, key, value);
}

b8 url_query_get(url_query_t* query, str_t key, str_t* dest) {
    return strhashmap_get(&query->values, key, dest);
}

b8 url_query_remove(url_query_t* query, str_t key) {
    return strhashmap_remove(&query->values, key);
}

string_t url_query_to_string(allocator_t* allocator, url_query_t* query) {
    if (!query) {
        LOG_ERROR("url_query_to_string - called with query 0.");
        return 0;
    }
    string_t string = string_from_parts(allocator, 0, query->values.length * 32);
    if (query->values.length == 0) {
        return string;
    }
    u64 i = 0;
    strhashmap_entry_t* curr = 0;
    do {
        curr = &(query->values.table[i++]);
        if (curr->is_occupied && string) {
            string = string_append_format(string, "%.*s=%.*s", curr->key.length, curr->key.data, curr->value.length, curr->value.data);
        }
    } while(i < query->values.capacity);
    return string;
}
