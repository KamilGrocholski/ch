#include "core/str.h"
#include "core/logger.h"
#include "core/defines.h"
#include "core/memory.h"
#include "net/http.h"

#include <stdio.h>
#include <assert.h>

void handle_makefile(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    http_response_send_file(response, HTTP_STATUS_OK, "./Makefile");
    return;
}

void handle_users(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr("hello user"));
    return;
}

void handle_user_id(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    str_t user_id = request->params[0];
    if (str_is_null_or_empty(user_id)) {
        http_response_send_text(response, HTTP_STATUS_BAD_REQUEST, str_from_cstr("No user id"));
        return;
    }
    char buffer[2000] = {0};
    sprintf(buffer, "%.*s", (i32)user_id.length, user_id.data);
    http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr(buffer));
    return;
}

void handle_void(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr("hello from void"));
    return;
}

int main() {
    memory_init();

    http_server_t server = {0};
    const u64 request_stack_buffer_size = MEBIBYTES(4);
    http_server_init(&server, request_stack_buffer_size);

    http_server_get(&server, "/makefile", handle_makefile);
    http_server_get(&server, "/users", handle_users);
    http_server_get(&server, "/users/:id", handle_user_id);
    http_server_get(&server, "/void", handle_void);

    const u16 port = 8080;
    http_server_start(&server, port);

    http_server_deinit(&server);

    memory_shutdown();

    return 0;
}
