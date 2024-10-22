#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"

void http_request_init(allocator_t* allocator, http_request_t* dest) {
    http_request_headers_init(allocator, &dest->headers);
}

void http_request_deinit(http_request_t* request) {
    http_request_headers_deinit(&request->headers);
}

b8 http_request_parse(str_t raw_request, http_request_t* dest) {
    ASSERT_MSG(dest, "http_request_parse - dest MUST not be 0.");

    b8 is_raw_request_null = str_is_null(raw_request);
    if (is_raw_request_null) {
        return false;
    }
    b8 is_raw_request_empty = str_is_empty(raw_request);
    if (is_raw_request_empty) {
        return false;
    }

    // Split headers and body
    str_t headers = str_pop_first_split(&raw_request, str_from_cstr("\n\n"));
    str_t body = raw_request;
    dest->body = body;

    // The first line is method, path and proto, e.g. 'GET / HTTP1.1'
    str_t header_line = str_pop_first_split_char(&headers, '\n');
    if (str_is_empty(header_line)) {
        return false;
    }
    str_t method = str_pop_first_split_char(&header_line, ' ');
    str_t path = str_pop_first_split_char(&header_line, ' ');
    str_t proto = header_line;
    if (!http_method_from_str(method, &dest->method)) {
        return false;
    }
    dest->path = path;
    dest->proto = proto;

    // Split key value pairs
    header_line = str_pop_first_split_char(&headers, '\n');
    str_t key;
    str_t value;
    while (!str_is_empty(header_line)) {
        key = str_pop_first_split(&header_line, str_from_cstr(": "));
        value = header_line;

        b8 should_parse_pair = !str_is_null(key) && !str_is_null(value) && !str_is_empty(key) && !str_is_empty(value);
        if (should_parse_pair) {
            if (!http_request_headers_set(&dest->headers, key, value)) {
                return false;
            }
        }
        header_line = str_pop_first_split_char(&headers, '\n');
    }

    return true;
}

static u64 _hash(str_t key, u64 capacity) {
    u64 hash = 0;
    for (u64 i = 0; i < key.length; i++) {
        hash = (hash * 31) + key.data[i];
    }
    return hash % capacity;
}

void http_request_headers_init(allocator_t* allocator, http_request_headers_t* headers) {
    if (!headers) {
        LOG_ERROR("http_request_headers_init - called with headers 0.");
        return;
    }
    headers->allocator = allocator;
    headers->capacity = 64;
    headers->length = 0;
    if (allocator) {
        headers->table = allocator->allocate(allocator->context, headers->capacity * sizeof(http_request_headers_entry_t));
    } else {
        headers->table = memory_allocate(headers->capacity * sizeof(http_request_headers_entry_t), MEMORY_TAG_HTTP_HEADERS);
    }
    if (!headers->table) {
        LOG_ERROR("http_request_headers_init - could not allocate headers table.");
        return;
    }
    memory_zero(headers->table, headers->capacity * sizeof(http_request_headers_entry_t));
}

static void http_request_headers_resize(http_request_headers_t* headers, u64 new_capacity) {
    if (new_capacity < HTTP_REQUEST_HEADERS_MIN_CAPACITY) {
        new_capacity = HTTP_REQUEST_HEADERS_MIN_CAPACITY;
    }
    http_request_headers_entry_t* old_table = headers->table;
    http_request_headers_entry_t* new_table = 0;
    u64 old_capacity = headers->capacity;
    headers->capacity = new_capacity;
    headers->length = 0;
    if (headers->allocator) {
        new_table = headers->allocator->allocate(headers->allocator->context, new_capacity * sizeof(http_request_headers_entry_t));
    } else {
        new_table = memory_allocate(new_capacity * sizeof(http_request_headers_entry_t), MEMORY_TAG_HTTP_HEADERS);
    }
    if (!new_table) {
        return;
    }
    headers->table = new_table;
    memory_zero(headers->table, new_capacity * sizeof(http_request_headers_entry_t));
    for (u64 i = 0; i < old_capacity; i++) {
        if (old_table[i].is_occupied) {
            http_request_headers_set(headers, old_table[i].key, old_table[i].value);
        }
    }
    if (headers->allocator) {
        headers->allocator->free(headers->allocator->context, old_table, old_capacity * sizeof(http_request_headers_entry_t));
    } else {
        memory_free(old_table, old_capacity * sizeof(http_request_headers_entry_t), MEMORY_TAG_HTTP_HEADERS);
    }
}

b8 http_request_headers_set(http_request_headers_t* headers, str_t key, str_t value) {
    if (headers->length >= (u64)(headers->capacity * HTTP_REQUEST_HEADERS_CAPACITY_THRESHHOLD)) {
        http_request_headers_resize(headers, headers->capacity * HTTP_REQUEST_HEADERS_GROW_FACTOR);
    }    
    u64 index = _hash(key, headers->capacity);
    u64 original_index = index;
    http_request_headers_entry_t* entry = &(headers->table[index]);
    while (entry->is_occupied && !str_compare(entry->key, key)) {
        index = (index + 1) % headers->capacity;
        if (index == original_index) {
            return false;
        }
        entry = &(headers->table[index]);
    }
    if (entry->is_occupied && str_compare(entry->key, key)) {
        if (value.length > entry->value.length) {
            if (headers->allocator) {
                const char* new_data = headers->allocator->allocate(headers->allocator->context, value.length);
                if (!new_data) {
                    return false;
                }
                headers->allocator->free(headers->allocator->context, (void*)value.data, value.length);
                entry->value.data = new_data;
            } else {
                const char* new_data = memory_allocate(value.length, MEMORY_TAG_HTTP_HEADERS);
                if (!new_data) {
                    return false;
                }
                memory_free((void*)value.data, value.length, MEMORY_TAG_HTTP_HEADERS);
                entry->value.data = new_data;
            }
        }
        memory_copy((void*)entry->value.data, value.data, value.length);
        entry->value.length = value.length;
        return true;
    };
    if (headers->allocator) {
        entry->key.data = headers->allocator->allocate(headers->allocator->context, key.length);
        entry->value.data = headers->allocator->allocate(headers->allocator->context, value.length);
    } else {
        entry->key.data = memory_allocate(key.length, MEMORY_TAG_HTTP_HEADERS);
        entry->value.data = memory_allocate(value.length, MEMORY_TAG_HTTP_HEADERS);
    }
    if (!entry->key.data || !entry->value.data) {
        return false;
    }
    memory_copy((void*)entry->key.data, key.data, key.length);
    entry->key.length = key.length;
    memory_copy((void*)entry->value.data, value.data, value.length);
    entry->value.length = value.length;
    entry->is_occupied = true;
    headers->length++;
    return true;
}

b8 http_request_headers_get(http_request_headers_t* headers, str_t key, str_t* value) {
    if (!headers || !headers->table) return false;
    u64 index = _hash(key, headers->capacity);
    u64 original_index = index;
    http_request_headers_entry_t* entry = &(headers->table[index]);
    while (entry->is_occupied) {
        if (str_compare(entry->key, key)) {
            *value = entry->value;
            return true;
        }
        index = (index + 1) % headers->capacity;
        if (index == original_index) return false;  // Full circle, key not found
        entry = &(headers->table[index]);
    }
    return false;
}

b8 http_request_headers_remove(http_request_headers_t* headers, str_t key) {
    if (!headers || !headers->table) return false;
    u64 index = _hash(key, headers->capacity);
    u64 original_index = index;

    while (headers->table[index].is_occupied) {
        if (str_compare(headers->table[index].key, key)) {
            headers->table[index].is_occupied = false;
            headers->length--;
            return true;
        }
        index = (index + 1) % headers->capacity;
        if (index == original_index) break;  // Full circle, key not found
    }

    return false;
}

void http_request_headers_deinit(http_request_headers_t* headers) {
    if (!headers || !headers->table) return;
    for (u64 i = 0; i < headers->capacity; i++) {
        http_request_headers_entry_t* e = &(headers->table[i]);
        if (headers->table[i].is_occupied) {
            if (headers->allocator) {
                headers->allocator->free(headers->allocator->context, (void*)e->key.data, e->key.length);
                headers->allocator->free(headers->allocator->context, (void*)e->value.data, e->value.length);
            } else {
                memory_free((void*)e->key.data, e->key.length, MEMORY_TAG_HTTP_HEADERS);
                memory_free((void*)e->value.data, e->value.length, MEMORY_TAG_HTTP_HEADERS);
            }
        }
    }
    if (headers->allocator) {
        headers->allocator->free(headers->allocator->context, headers->table, headers->capacity * sizeof(http_request_headers_entry_t));
    } else {
        memory_free(headers->table, headers->capacity * sizeof(http_request_headers_entry_t), MEMORY_TAG_HTTP_HEADERS);
    }
    headers->table = 0;
    headers->length = 0;
    headers->capacity = 0;
}

