#pragma once

#include "net/http/method.h"

#include "core/hashmap.h"
#include "core/memory.h"
#include "core/str.h"

typedef struct http_request_headers_entry_t {
    str_t key;
    str_t value;
    b8 is_occupied;
} http_request_headers_entry_t;

typedef struct http_request_headers_t {
    http_request_headers_entry_t* table;
    u64 capacity;
    u64 length;
    allocator_t* allocator;
} http_request_headers_t;

typedef struct http_request_t {
    http_method_t method;
    str_t proto;
    http_request_headers_t* headers; 
    str_t body;
} http_request_t;

void http_request_init(allocator_t* allocator, http_request_t* dest);

void http_request_deinit(http_request_t* request);

b8 http_request_parse(str_t raw_request, http_request_t* dest);

void http_request_headers_init(allocator_t* allocator, http_request_headers_t* headers);

void http_request_headers_deinit(http_request_headers_t* headers);

b8 http_request_headers_set(http_request_headers_t* headers, str_t key, str_t value);

b8 http_request_headers_get(http_request_headers_t* headers, str_t key, str_t* value);

b8 http_request_headers_remove(http_request_headers_t* headers, str_t key);
