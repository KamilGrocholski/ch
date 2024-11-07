#pragma once

#include "core/defines.h"
#include "core/str.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/strhashmap.h"
#include "core/hashmap.h"

#include <stdarg.h>

typedef struct http_result_t {
    b8 ok;
} http_result_t;

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
    http_request_t* request;
    http_status_t status;
    strhashmap_t headers;
    str_t body;
} http_response_t;

b8 http_response_init(http_response_t* response);

void http_response_deinit(http_response_t* response);

b8 http_response_headers_set(http_response_t* response, str_t key, str_t value);

b8 http_response_status_set(http_response_t* response, http_status_t status);

http_result_t http_response_send_no_content(http_response_t* response, http_status_t status);

http_result_t http_response_send_text(http_response_t* response, http_status_t status, str_t text);

http_result_t http_response_send_file(http_response_t* response, http_status_t status, const char* path);

http_result_t http_response_send(http_response_t* response, http_status_t status, str_t content);

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

typedef http_result_t (*http_handler_t)(http_response_t* response, http_request_t* request);

void http_request_init(allocator_t* allocator, http_request_t* dest);

void http_request_deinit(http_request_t* request);

string_t http_request_to_string(allocator_t* allocator, http_request_t* request);

b8 http_request_parse(str_t raw_request, http_request_t* dest);
// -- request/response end

// -- middleware start
typedef struct http_middleware_container_t http_middleware_container_t;
typedef struct http_server_t http_server_t;

typedef http_result_t (*http_middleware_t)(http_response_t* response, http_request_t* request, http_handler_t next);

http_middleware_container_t* http_middleware_containers_from_v(u64 middleware_count, va_list middlewares);

http_result_t http_middleware_containers_apply_all(
    http_response_t* response,
    http_request_t* request,
    http_middleware_container_t* middleware_containers,
    http_handler_t final_handler
);

http_result_t http_process_all(
    http_response_t* response, 
    http_request_t* request, 
    http_middleware_container_t* middleware_containers,
    http_handler_t final_handler
);
// -- middleware end

// -- router start
#define HTTP_ROUTER_CHILDREN_MAX_CAPACITY (127)

typedef struct http_middleware_container_t { 
    http_middleware_t middleware;
} http_middleware_container_t;

typedef struct http_method_handler_t {
    http_handler_t handler;
    http_middleware_container_t* middleware_containers;
} http_method_handler_t;

typedef struct http_router_node_t {
    str_t segment; // static | param | wildcard
    http_method_handler_t method_handlers[_HTTP_METHOD_MAX];
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

http_method_handler_t http_router_search(http_router_t* router, http_method_t method, str_t path, str_t (*out_params)[]);

void http_router_add(
    http_router_t* router,
    http_method_t method, 
    const char* path, 
    http_handler_t handler, 
    u64 middlewares_count,
    va_list middlewares
);

void http_router_use(
    http_router_t* router,
    http_method_t method, 
    const char* path, 
    http_handler_t handler, 
    u64 middlewares_count,
    va_list middlewares
);
// -- router end

// -- server start
typedef struct http_server_t {
    http_router_t router;
    u32 stack_buffer_size;
    http_middleware_container_t* middleware_containers;
    http_handler_t handle_not_found;
    http_handler_t handle_internal_server_error;
} http_server_t;

http_result_t http_server_process_request(
    http_server_t* server, 
    http_response_t* response,
    http_request_t* request,
    http_middleware_container_t* middleware_containers,
    http_handler_t final_handler
);

void http_server_init(http_server_t* server, u32 stack_buffer_size);

void http_server_deinit(http_server_t* server);

void http_server_start(http_server_t* server, u16 port);

void http_server_not_found(http_server_t* server, http_handler_t handler);

void http_server_internal_server_error(http_server_t* server, http_handler_t handler);

void http_server_use(http_server_t* server, u64 middlewares_count, ...);

void http_server_get(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...);

void http_server_post(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...);

void http_server_delete(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...);
// -- server end

// -- mime start
typedef enum mime_t {
    MIME_APPLICATION_HTML,
    MIME_APPLICATION_JSON,
    MIME_APPLICATION_JAVASCRIPT,
    MIME_APPLICATION_STAR,
    MIME_TEXT_JAVASCRIPT,
    MIME_TEXT_PLAIN,
    MIME_TEXT_PLAIN_UTF8,
    MIME_TEXT_HTML,
    MIME_TEXT_HTML_UTF8,
    MIME_TEXT_CSS,
    MIME_TEXT_CSS_UTF8,
    MIME_TEXT_STAR,
    MIME_LENGTH,
} mime_t;

b8 mime_is_valid(mime_t mime);

b8 mime_to_str(mime_t mime, str_t* dest);

b8 mime_from_str(str_t str, mime_t* dest);
// -- mime end

// -- cookie start
typedef enum cookie_same_site_t {
    COOKIE_SAME_SITE_DEFAULT_MODE = 1,
    COOKIE_SAME_SITE_LAX_MODE,
    COOKIE_SAME_SITE_STRICT_MODE,
    COOKIE_SAME_SITE_NONE_MODE,
} cookie_same_site_t;

typedef struct cookie_t {
    str_t name;
    str_t value;

    str_t path;
    str_t domain;
    // TODO: add expires time_t;
    str_t raw_expires;

    // 0 means no 'Max-Age' attribute specified.
    // <0 means delete cookie now, equivalently 'Max-Age: 0'
    // >0 means Max-Age attribute present and given in seconds
    i32 max_age;
    b8 secure;
    b8 http_only;
    cookie_same_site_t same_site;
    str_t raw;
} cookie_t;

typedef struct cookies_t {
    strhashmap_t strhashmap;
} cookies_t;

b8 cookies_parse_cookie_request_list(str_t list, cookies_t* cookies);

void cookies_init(cookies_t* cookies);

void cookies_deinit(cookies_t* cookies);

b8 cookies_get(cookies_t* cookies, str_t key, str_t* out_value);

b8 cookies_set(cookies_t* cookies, str_t key, str_t value);

b8 cookies_delete(cookies_t* cookies, str_t key);

string_t cookies_to_string(allocator_t* allocator, cookies_t* cookies);
// -- cookie end
