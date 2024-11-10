#include "net/http.h"

#include "core/logger.h"
#include "core/array.h"
#include "core/assert.h"

http_result_t http_middleware_containers_apply_all(
    http_response_t* response, 
    http_request_t* request, 
    http_middleware_container_t* middleware_containers
) {
    if (!middleware_containers) {
        LOG_DEBUG("http_middleware_containers_apply_all - no middlewares to apply");
        return HTTP_RESULT_NEXT;
    }
    LOG_DEBUG("http_middleware_containers_apply_all - starting to apply: %llu middlewares", array_length(middleware_containers));
    for (u64 i = 0; i < array_length(middleware_containers); i++) {
        http_middleware_t current_middleware = middleware_containers[i].middleware;
        if (current_middleware) {
            LOG_DEBUG("http_middleware_containers_apply_all - calling middleware at index: %llu", i);
            http_result_t result = current_middleware(response, request);
            if (result.type != HTTP_RESULT_TYPE_NEXT) {
                LOG_DEBUG("http_middleware_containers_apply_all - middleware at index %llu stopped the chain", i);
                return HTTP_RESULT_SEND;
            }
        } else {
            LOG_FATAL("http_middleware_containers_apply_all - 0 middleware at index: %llu", i);
        }
    }
    return HTTP_RESULT_NEXT;
}

http_middleware_container_t* http_middleware_containers_from_v(u64 middleware_count, va_list middlewares) {
    http_middleware_container_t* middleware_containers = 0;
    if (middleware_count) {
        middleware_containers = array_create(0, http_middleware_container_t);
        ASSERT(middleware_containers);
        for (u64 i = 0; i < middleware_count; i++) {
            http_middleware_t middleware = va_arg(middlewares, http_middleware_t);
            ASSERT(middleware);
            array_append(middleware_containers, (http_middleware_container_t){.middleware = middleware});
            ASSERT(middleware_containers);
        }
    }
    return middleware_containers;
}
