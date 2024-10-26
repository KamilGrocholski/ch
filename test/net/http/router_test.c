#include "router_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <core/array.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <stdio.h>

void handle_users(http_response_writer_t writer, http_request_t* request) {
    (void)request;
}

u8 http_router__should_register_route_and_find_handler(void) {
    http_router_t router = {0};
    http_router_init(&router);

    http_router_add(&router, HTTP_METHOD_GET, "/users/:id/friends", handle_users);

    strhashmap_t params = {0};
    strhashmap_init(0, &params);
    http_handler_t handler = http_router_search(&router, HTTP_METHOD_GET, str_from_cstr("users/234/friends"), &params);
    expect_neq(0, handler);
    http_request_t request;
    http_request_init(0, &request);
    request.method = HTTP_METHOD_GET;
    request.path = str_from_cstr("users");
    request.proto = str_from_cstr("HTTP1.1");
    request.body = str_from_cstr("body");
    request.params = params;
    str_t got;
    b8 ok = strhashmap_get(&params, str_from_cstr("id"), &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "234");
    handler((http_response_writer_t){0}, &request);

    http_router_deinit(&router);

    return true;
}

void http_router__register_test(void) {
    test_manager_register(http_router__should_register_route_and_find_handler, "Http router should register route and find handler");
}
