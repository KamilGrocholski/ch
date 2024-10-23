#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/str.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

static void handle_client(http_server_t* server, i32 socket) {
    i32 client_fd = accept(socket, 0, 0);

    char buffer[server->stack_buffer_size];
    i64 recv_length = recv(client_fd, buffer, server->stack_buffer_size, 0);
    if (recv_length <= 0) {
        close(client_fd);
        return;
    }
    str_t raw_request = str_from_parts(buffer, (u64)recv_length);

    http_request_t request = {0};
    http_request_init(0, &request);

    if (!http_request_parse(raw_request, &request)) {
        LOG_ERROR("handle_client - http request parsing failed");
        goto cleanup;
    }
    http_handler_t handler = http_router_search(&server->router, request.method, request.path);
    if (!handler) {
        LOG_ERROR("TODO - add 404");
    } else {
        handler((http_response_writer_t){0}, &request);
    }

cleanup: 
    http_request_deinit(&request);
    close(client_fd);
}

void http_server_init(http_server_t* server, u32 stack_buffer_size) {
    server->stack_buffer_size = stack_buffer_size;
    http_router_init(&server->router);
}

void http_server_deinit(http_server_t* server) {
    http_router_deinit(&server->router);
}

void http_server_start(http_server_t* server, u16 port) {
    ASSERT_MSG(server, "http_server_start - called with server 0.");

    i32 sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(sock, &addr, sizeof(addr));

    listen(sock, 10);

    while(1) {
        handle_client(server, sock);
    }

    close(sock);
}

void http_server_get(http_server_t* server, const char* path, http_handler_t handler) {
    http_router_add(&server->router, HTTP_METHOD_GET, path, handler);
}

void http_server_post(http_server_t* server, const char* path, http_handler_t handler) {
    http_router_add(&server->router, HTTP_METHOD_POST, path, handler);
}

void http_server_delete(http_server_t* server, const char* path, http_handler_t handler) {
    http_router_add(&server->router, HTTP_METHOD_DELETE, path, handler);
}
