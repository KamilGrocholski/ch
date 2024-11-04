#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/str.h"
#include "core/memory.h"
#include "core/strhashmap.h"
#include "core/array.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

static http_result_t default_handle_not_found(http_response_t* response, http_request_t* request) {
    (void)request;
    return http_response_send_no_content(response, HTTP_STATUS_NOT_FOUND);
}

static http_result_t default_handle_internal_server_error(http_response_t* response, http_request_t* request) {
    (void)request;
    return http_response_send_no_content(response, HTTP_STATUS_INTERNAL_SERVER_ERROR);
}

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
    http_response_t response = {0};
    http_response_init(&response);
    response.client_fd = client_fd;
    response.request = &request;
    http_method_handler_t method_handler = http_router_search(&server->router, request.method, request.path, &request.params);
    http_result_t result;
    if (!method_handler.handler) {
        LOG_DEBUG("handle_client - handle not found");
        if (server->handle_not_found) {
            LOG_DEBUG("handle_client - cutom not found");
            result = server->handle_not_found(&response, &request);
        } else {
            LOG_DEBUG("handle_client - default not found");
            result = default_handle_not_found(&response, &request);
        }
    } else {
        result = http_server_process_request(
            server,
            &response, 
            &request, 
            method_handler.middleware_containers,
            method_handler.handler
        );
    }
    if (!result.ok) {
        LOG_ERROR("handle_client - result is not ok");
        if (server->handle_internal_server_error) {
            result = server->handle_internal_server_error(&response, &request);
        } else {
            result = default_handle_internal_server_error(&response, &request);
        }
    }

cleanup: 
    http_request_deinit(&request);
    http_response_deinit(&response);
    close(client_fd);
    memory_report();
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

void http_server_not_found(http_server_t* server, http_handler_t handler) {
    server->handle_not_found = handler;
}

void http_server_internal_server_error(http_server_t* server, http_handler_t handler) {
    server->handle_internal_server_error = handler;
}

void http_server_use(http_server_t* server, u64 middlewares_count, ...) {
    if (!middlewares_count) {
        return;
    }
    if (!server->middleware_containers) {
        server->middleware_containers = array_create(0, http_middleware_container_t);
        ASSERT(server->middleware_containers);
    }
    va_list args;
    va_start(args, middlewares_count);
    http_middleware_t middleware = va_arg(args, http_middleware_t);
    array_append(server->middleware_containers, (http_middleware_container_t){.middleware = middleware});
    ASSERT(server->middleware_containers);
    va_end(args);
}

void http_server_get(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...) {
    va_list args;
    va_start(args, middlewares_count);
    http_router_add(&server->router, HTTP_METHOD_GET, path, handler, middlewares_count, args);
    va_end(args);
}

void http_server_post(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...) {
    va_list args;
    va_start(args, middlewares_count);
    http_router_add(&server->router, HTTP_METHOD_POST, path, handler, middlewares_count, args);
    va_end(args);
}

void http_server_delete(http_server_t* server, const char* path, http_handler_t handler, u64 middlewares_count, ...) {
    va_list args;
    va_start(args, middlewares_count);
    http_router_add(&server->router, HTTP_METHOD_DELETE, path, handler, middlewares_count, args);
    va_end(args);
}

http_result_t http_server_process_request(
    http_server_t* server, 
    http_response_t* response,
    http_request_t* request,
    http_middleware_container_t* middleware_containers,
    http_handler_t final_handler
) {
    http_result_t result = http_middleware_containers_apply_all(response, request, server->middleware_containers, final_handler);
    if (!result.ok) {
        return result;
    }
    result = http_middleware_containers_apply_all(response, request, middleware_containers, final_handler);
    if (!result.ok) {
        return result;
    }
    return final_handler(response, request);
}
