#include "router_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <core/array.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <stdio.h>

void handle_user_id(http_response_t* response, http_request_t* request) {
    (void)request;
}

void handle_user_id_okej(http_response_t* response, http_request_t* request) {
    (void)request;
}

void handle_user_id_friends(http_response_t* response, http_request_t* request) {
    (void)request;
}

u8 http_router__should_register_route_and_find_handler(void) {
    http_router_t router = {0};
    http_router_init(&router);

    http_router_add(&router, HTTP_METHOD_GET, "/users/:id/n/:age", handle_user_id_okej);
    http_router_add(&router, HTTP_METHOD_GET, "/users/:id", handle_user_id);
    http_router_add(&router, HTTP_METHOD_GET, "/users/:id/friends", handle_user_id_friends);
    http_router_add(&router, HTTP_METHOD_GET, "/users/:id/okej", handle_user_id_okej);
    http_router_add(&router, HTTP_METHOD_GET, "/users/:id/:age", handle_user_id_okej);
    http_router_add(&router, HTTP_METHOD_GET, "/void/:id", handle_user_id_okej);
    http_router_add(&router, HTTP_METHOD_GET, "/void", handle_user_id_okej);

    http_request_t request = {0};
    http_request_init(0, &request);
    http_response_t response = {0};
    http_response_init(&response);
    response.origin_request = &request;
    http_handler_t handler = http_router_search(&router, HTTP_METHOD_GET, str_from_cstr("users/234/friends"), &request.params);
    expect_neq(0, handler);
    expect_eq(handle_user_id_friends, handler);
    request.method = HTTP_METHOD_GET;
    request.path = str_from_cstr("users");
    request.proto = str_from_cstr("HTTP1.1");
    request.body = str_from_cstr("body");
    expect_str_eq_cstr(request.params[0], "234");
    handler(&response, &request);

    http_router_deinit(&router);

    return true;
}

void http_router__register_test(void) {
    test_manager_register(http_router__should_register_route_and_find_handler, "Http router should register route and find handler");
}
