#pragma once

#include "core/defines.h"
#include "core/str.h"
#include "core/memory.h"
#include "core/strhashmap.h"

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

typedef struct http_request_t {
    http_method_t method;
    str_t path;
    str_t proto;
    strhashmap_t headers; 
    str_t body;
} http_request_t;

typedef struct http_response_writer_t {
} http_response_writer_t;

typedef void (*http_handler_t)(http_response_writer_t writer, http_request_t* request);

void http_request_init(allocator_t* allocator, http_request_t* dest);

void http_request_deinit(http_request_t* request);

b8 http_request_parse(str_t raw_request, http_request_t* dest);
// -- request end

// -- router start
#define HTTP_ROUTER_PATTERN_SEGMENTS_MAX_CAPACITY (32)

typedef struct http_router_segment_t {
    str_t literal;
    b8 is_wildcard;
} http_router_segment_t;

typedef struct http_router_pattern_t {
    str_t src;
    http_router_segment_t segments[HTTP_ROUTER_PATTERN_SEGMENTS_MAX_CAPACITY];
    u64 segments_length;
} http_router_pattern_t;

#define HTTP_ROUTER_NODE_MAX_CAPACITY (127)

typedef struct http_router_node_t {
    http_router_pattern_t pattern;
    http_handler_t handlers[_HTTP_METHOD_MAX];
    struct http_router_node_t* children;
} http_router_node_t;

typedef struct http_router_t {
    http_router_node_t root;
} http_router_t;

void http_router_init(http_router_t* router);

void http_router_deinit(http_router_t* router);

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t path, strhashmap_t* out_params);

void http_router_add(http_router_t* router, http_method_t method, const char* path, http_handler_t handler);
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
