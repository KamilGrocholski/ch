#include "router_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <core/array.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <stdio.h>
#include <stdarg.h>

static void add_testing_route(
    http_router_t* router,
    http_method_t method, 
    const char* path, 
    http_handler_t handler, 
    u64 middlewares_count, 
    ...
) {
    va_list args;
    va_start(args, middlewares_count);
    http_router_add(router, method, path, handler, middlewares_count, args);
    va_end(args);
}

http_result_t middleware_after_void(http_response_t* response, http_request_t* request, http_handler_t next) {
    LOG_DEBUG("middleware after void");
    return next(response, request);
}

http_result_t middleware_void(http_response_t* response, http_request_t* request, http_handler_t next) {
    LOG_DEBUG("middleware void");
    return next(response, request);
}

http_result_t handle_user_id(http_response_t* response, http_request_t* request) {
    (void)response;
    (void)request;
    return (http_result_t){.ok = true};
}

http_result_t handle_user_id_okej(http_response_t* response, http_request_t* request) {
    (void)response;
    (void)request;
    return (http_result_t){.ok = true};
}

http_result_t handle_user_id_friends(http_response_t* response, http_request_t* request) {
    (void)response;
    (void)request;
    return (http_result_t){.ok = true};
}

u8 http_router__should_register_route_and_find_handler(void) {
    http_router_t router = {0};
    http_router_init(&router);

    add_testing_route(&router, HTTP_METHOD_GET, "/users/:id/n/:age", handle_user_id_okej, 0);
    add_testing_route(&router, HTTP_METHOD_GET, "/users/:id", handle_user_id, 0);
    add_testing_route(&router, HTTP_METHOD_GET, "/users/:id/friends", handle_user_id_friends, 2, middleware_void, middleware_after_void);
    add_testing_route(&router, HTTP_METHOD_GET, "/users/:id/okej", handle_user_id_okej, 0);
    add_testing_route(&router, HTTP_METHOD_GET, "/users/:id/:age", handle_user_id_okej, 0);
    add_testing_route(&router, HTTP_METHOD_GET, "/void/:id", handle_user_id_okej, 0);
    add_testing_route(&router, HTTP_METHOD_GET, "/void", handle_user_id_okej, 0);

    http_request_t request = {0};
    http_request_init(0, &request);
    http_response_t response = {0};
    http_response_init(&response);
    response.request = &request;
    http_method_handler_t method_handler = http_router_search(&router, HTTP_METHOD_GET, str_from_cstr("users/234/friends"), &request.params);
    expect_neq(0, method_handler.handler);
    expect_eq(handle_user_id_friends, method_handler.handler);
    request.method = HTTP_METHOD_GET;
    request.path = str_from_cstr("users");
    request.proto = str_from_cstr("HTTP1.1");
    request.body = str_from_cstr("body");
    expect_str_eq_cstr(request.params[0], "234");
    http_result_t result = http_process_all(
        &response, 
        &request, 
        method_handler.middleware_containers, 
        method_handler.handler
    );
    expect_true(result.ok);

    http_router_deinit(&router);

    return true;
}

void http_router__register_test(void) {
    test_manager_register(http_router__should_register_route_and_find_handler, "Http router should register route and find handler");
}
