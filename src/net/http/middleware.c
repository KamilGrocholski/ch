#include "net/http.h"

#include "core/logger.h"

http_result_t http_middleware_process_chain(
    http_response_t* response,
    http_request_t* request,
    http_middleware_container_t* middleware_containers,
    u64 middleware_count,
    http_handler_t final_handler
) {
    LOG_DEBUG("http_middleware_process_chain - starting with count: %llu", middleware_count);
    for (u64 i = 0; i < middleware_count; i++) {
        http_middleware_t current_middleware = middleware_containers[i].middleware;
        if (current_middleware) {
            LOG_DEBUG("http_middleware_process_chain - calling middleware at index: %llu", i);
            http_result_t result = current_middleware(response, request, final_handler);
            if (!result.ok) {
                LOG_DEBUG("Middleware at index %llu stopped the chain.", i);
                return result;
            }
        } else {
            LOG_FATAL("http_middleware_process_chain - skipping 0 middleware at index: %llu", i);
        }
    }
    LOG_DEBUG("http_middleware_process_chain - all middlewares processed successfully");
    return final_handler(response, request);
}
