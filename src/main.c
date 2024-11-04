#include "core/str.h"
#include "core/logger.h"
#include "core/defines.h"
#include "core/memory.h"
#include "core/assert.h"
#include "net/http.h"
#include "fs/fs.h"
#include "env/env.h"

#include <string.h>
#include <stdio.h>

http_result_t middleware_void(http_response_t* response, http_request_t* request, http_handler_t next) {
    LOG_INFO("middleware_void - starting: %s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    return next(response, request);
}

http_result_t handle_makefile(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    return http_response_send_file(response, HTTP_STATUS_OK, "./Makefile");
}

http_result_t handle_users(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    return http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr("hello user"));
}

http_result_t handle_user_id(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    str_t user_id = request->params[0];
    if (str_is_null_or_empty(user_id)) {
        return http_response_send_text(response, HTTP_STATUS_BAD_REQUEST, str_from_cstr("No user id"));
    }
    char buffer[2000] = {0};
    sprintf(buffer, "%.*s", (i32)user_id.length, user_id.data);
    return http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr(buffer));
}

http_result_t handle_void(http_response_t* response, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length,
            request->path.data);

    return http_response_send_text(response, HTTP_STATUS_OK, str_from_cstr("hello from void"));
}

int main() {
    memory_init();

    env_t env = {0};
    ASSERT(env_load(0, ".env", &env));

    http_server_t server = {0};
    const u64 request_stack_buffer_size = MEBIBYTES(4);
    http_server_init(&server, request_stack_buffer_size);

    http_server_get(&server, "/makefile", handle_makefile, 0);
    http_server_get(&server, "/users", handle_users, 1, middleware_void);
    http_server_get(&server, "/users/:id/void", handle_void, 0);
    http_server_get(&server, "/users/:id", handle_user_id, 0);
    http_server_get(&server, "/void", handle_void, 0);

    LOG_INFO("Server listening on port %llu", env.PORT);
    http_server_start(&server, env.PORT);

    http_server_deinit(&server);

    memory_shutdown();

    return 0;
}
