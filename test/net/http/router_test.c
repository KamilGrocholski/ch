#include "router_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <stdio.h>

void handle_users(http_response_writer_t writer, http_request_t* request) {
    (void)request;
}

u8 http_router__should_register_route_and_find_handler(void) {
    http_router_t router;
    http_router_init(&router);

    http_router_add(&router, HTTP_METHOD_GET, "/users", handle_users);

    http_handler_t handler = http_router_search(&router, HTTP_METHOD_GET, str_from_cstr("/users"));
    expect_neq(0, handler);
    handler((http_response_writer_t){0}, 0);

    return true;
}

void http_router__register_test(void) {
    test_manager_register(http_router__should_register_route_and_find_handler, "Http router should register route and find handler");
}
