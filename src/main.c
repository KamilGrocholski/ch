#include "core/string.h"
#include "core/logger.h"
#include "core/defines.h"
#include "core/array.h"
#include "core/hashmap.h"
#include "core/assert.h"
#include "fs/fs.h"
#include "net/http.h"

#include <stdio.h>
#include <assert.h>

void handle_users(http_response_writer_t writer, http_request_t* request) {
    LOG_INFO("%s %.*s",
            http_method_to_cstr(request->method),
            request->path.length, 
            request->path.data);
}

int main() {
    memory_init();

    http_server_t server = {0};
    const u64 request_stack_buffer_size = 4 * 1024 * 1024;
    http_server_init(&server, request_stack_buffer_size);

    http_server_get(&server, "/users", handle_users);

    const u16 port = 8080;
    http_server_start(&server, port);

    http_server_deinit(&server);

    memory_shutdown();

    return 0;
}
