#pragma once

#include "core/defines.h"
#include "core/str.h"
#include "core/memory.h"

typedef struct http_request_t http_request_t;

// -- handler start
typedef struct http_response_writer_t {
} http_response_writer_t;

typedef void (*http_handler_t)(http_response_writer_t writer, http_request_t* request);
// -- handler end

// -- method start
typedef enum http_method_t {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_DELETE,
    _HTTP_METHOD_MAX,
} http_method_t;

b8 http_method_from_str(str_t str, http_method_t* dest);

b8 http_method_from_cstr(const char* cstr, http_method_t* dest);

str_t http_method_to_str(http_method_t method);

const char* http_method_to_cstr(http_method_t method);
// -- method end

// -- request start
#define HTTP_REQUEST_HEADERS_MIN_CAPACITY (32)
#define HTTP_REQUEST_HEADERS_GROW_FACTOR (2)
#define HTTP_REQUEST_HEADERS_CAPACITY_THRESHHOLD (0.75f)

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
    str_t path;
    str_t proto;
    http_request_headers_t headers; 
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
// -- request end

// -- router start
typedef struct trie_node_t {
    struct trie_node_t* children[128];
    http_handler_t handlers[_HTTP_METHOD_MAX];
} trie_node_t;

trie_node_t* trie_node_create();

void trie_insert(trie_node_t* root, http_method_t method, const char* key, http_handler_t handler);

http_handler_t trie_search(trie_node_t* root, http_method_t method, str_t key);

typedef struct http_router_t {
    trie_node_t* root;
} http_router_t;

void http_router_init(http_router_t* router);

void http_router_deinit(http_router_t* router);

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t key);

void http_router_add(http_router_t* router, http_method_t method, const char*, http_handler_t handler);
// -- router end

// -- server start
typedef struct http_server_t {
    http_router_t router;
    u32 stack_buffer_size;
} http_server_t;

void http_server_init(http_server_t* server, u32 stack_buffer_size);

void http_server_deinit(http_server_t* server);

void http_server_start(http_server_t* server, u16 port);

void http_server_get(http_server_t* server, const char* path, http_handler_t handler);

void http_server_post(http_server_t* server, const char* path, http_handler_t handler);

void http_server_delete(http_server_t* server, const char* path, http_handler_t handler);
// -- server end
