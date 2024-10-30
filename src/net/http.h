#pragma once

#include "core/defines.h"
#include "core/str.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/strhashmap.h"

// -- status start
typedef enum http_status_t {
    // 200s
    HTTP_STATUS_OK                    = 200,
    HTTP_STATUS_CREATED               = 201,
    HTTP_STATUS_ACCEPTED              = 202,
    HTTP_STATUS_NON_AUTHORITATIVE_INFO = 203,
    HTTP_STATUS_NO_CONTENT            = 204,

    // 300s
    HTTP_STATUS_FOUND                 = 302,
    HTTP_STATUS_SEE_OTHER             = 303,
    HTTP_STATUS_NOT_MODIFIED          = 304,

    // 400s
    HTTP_STATUS_BAD_REQUEST           = 400,
    HTTP_STATUS_UNAUTHORIZED          = 401,
    HTTP_STATUS_FORBIDDEN             = 403,
    HTTP_STATUS_NOT_FOUND             = 404,
    HTTP_STATUS_METHOD_NOT_ALLOWED    = 405,
    HTTP_STATUS_NOT_ACCEPTABLE        = 406,
    HTTP_STATUS_REQUEST_TIMEOUT       = 408,
    HTTP_STATUS_CONFLICT              = 409,
    HTTP_STATUS_TOO_MANY_REQUESTS     = 429,

    // 500s
    HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
} http_status_t;

const char* http_status_to_cstr(http_status_t status);

str_t http_status_to_str(http_status_t status);
// -- status end

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

// -- request/response start
typedef struct http_request_t http_request_t;

typedef struct http_response_t {
    i32 client_fd;
    http_request_t* origin_request;
    http_status_t status;
    strhashmap_t headers;
    str_t body;
} http_response_t;

b8 http_response_init(http_response_t* response);

void http_response_deinit(http_response_t* response);

b8 http_response_headers_set(http_response_t* response, str_t key, str_t value);

b8 http_response_status_set(http_response_t* response, http_status_t status);

b8 http_response_send_no_content(http_response_t* response, http_status_t status);

b8 http_response_send_text(http_response_t* response, http_status_t status, str_t text);

b8 http_response_send(http_response_t* response, http_status_t status, str_t data);

#define HTTP_REQUEST_HEADERS_MIN_CAPACITY (32)
#define HTTP_REQUEST_HEADERS_GROW_FACTOR (2)
#define HTTP_REQUEST_HEADERS_CAPACITY_THRESHHOLD (0.75f)
#define HTTP_REQUEST_PARAMS_MAX_CAPACITY (32)

typedef struct http_request_t {
    http_method_t method;
    str_t path;
    str_t proto;
    strhashmap_t headers; 
    str_t body;
    str_t params[HTTP_REQUEST_PARAMS_MAX_CAPACITY];
} http_request_t;

typedef void (*http_handler_t)(http_response_t *response, http_request_t* request);

void http_request_init(allocator_t* allocator, http_request_t* dest);

void http_request_deinit(http_request_t* request);

string_t http_request_to_string(allocator_t* allocator, http_request_t* request);

b8 http_request_parse(str_t raw_request, http_request_t* dest);
// -- request/response end

// -- router start
#define HTTP_ROUTER_CHILDREN_MAX_CAPACITY (127)

typedef struct http_router_node_t {
    str_t segment; // static | param | wildcard
    http_handler_t method_handlers[_HTTP_METHOD_MAX];
    struct http_router_node_t* children[HTTP_ROUTER_CHILDREN_MAX_CAPACITY];
    b8 is_param; // e.g. ':id'
    b8 is_wildcard; // e.g. '*'
} http_router_node_t;

string_t http_router_node_to_string(http_router_node_t* node, str_t path_prefix);

typedef struct http_router_t {
    http_router_node_t root;
} http_router_t;

void http_router_init(http_router_t* router);

void http_router_deinit(http_router_t* router);

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t path, str_t* out_params);

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
