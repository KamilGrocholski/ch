#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/string.h"
#include "core/array.h"
#include "core/strhashmap.h"

#include <stdarg.h>

static b8 http_router_add_route(http_router_t* router, http_method_t method, str_t path, http_handler_t handler, 
    http_middleware_container_t* middleware_containers);
static http_router_node_t* http_router_match(http_router_t* router, http_method_t method, str_t path, str_t (*out_params)[]);

void http_router_init(http_router_t* router) {
    (void)router;
}

void http_router_deinit(http_router_t* router) {
    if (!router) {
        LOG_ERROR("http_router_deinit - called with router 0.");
        return;
    }
    (void)router;
}

http_method_handler_t http_router_search(http_router_t* router, http_method_t method, str_t path, str_t (*out_params)[]) {
    http_router_node_t* node = http_router_match(router, method, path, out_params);
    if (!node) {
        LOG_DEBUG("http_router_search - handler NOT found for [%s %.*s]", http_method_to_cstr(method), path.length, path.data);
        return (http_method_handler_t){0};
    }
    http_method_handler_t method_handler = node->method_handlers[method];
    LOG_DEBUG("http_router_search - handler found for [%s %.*s]", http_method_to_cstr(method), path.length, path.data);
    return method_handler;
}

void http_router_add(
    http_router_t* router, 
    http_method_t method, 
    const char* path, 
    http_handler_t handler, 
    u64 middlewares_count,
    va_list middlewares
) {
    ASSERT(router);
    ASSERT(method >= 0 && method < _HTTP_METHOD_MAX);
    ASSERT(path);
    ASSERT(handler);

    http_middleware_container_t* middleware_containers = http_middleware_containers_from_v(middlewares_count, middlewares);

    ASSERT(http_router_add_route(router, method, str_from_cstr(path), handler, middleware_containers));
}

static http_router_node_t* http_router_match(http_router_t* router, http_method_t method, str_t path, str_t (*out_params)[]) {
    LOG_DEBUG("http_router_match - matching [%s %.*s]", http_method_to_cstr(method), path.length, path.data);
    
    http_router_node_t* curr = &router->root;
    str_t rest = path;
    if (str_has_prefix(rest, str_from_cstr("/"))) {
        rest = str_cut_prefix(rest, str_from_cstr("/"));
    }
    u64 param_index = 0;

    while (!str_is_empty(rest)) {
        str_t segment = str_pop_first_split_char(&rest, '/');
        b8 found = false;
        http_router_node_t* param_match = 0;
        http_router_node_t* wildcard_match = 0;

        for (u64 i = 0; i < HTTP_ROUTER_CHILDREN_MAX_CAPACITY && curr->children[i]; i++) {
            http_router_node_t* child = curr->children[i];
            
            if (str_compare(child->segment, segment)) {
                curr = child;
                found = true;
                break;
            }
            if (child->is_param) {
                param_match = child;
            }
            if (child->is_wildcard) {
                wildcard_match = child;
            }
        }

        if (!found && param_match) {
            curr = param_match;
            if (param_index >= HTTP_REQUEST_PARAMS_MAX_CAPACITY) {
                return 0;
            }
            (*out_params)[param_index++] = segment;
            found = true;
        }
        
        if (!found && wildcard_match) {
            curr = wildcard_match;
            if (param_index >= HTTP_REQUEST_PARAMS_MAX_CAPACITY) {
                return 0;
            }
            (*out_params)[param_index++] = rest;
            found = true;
            break;
        }

        if (!found) {
            LOG_DEBUG("http_router_match - no match found for segment [%.*s]", segment.length, segment.data);
            return 0;
        }
    }

    if (curr->method_handlers[method].handler) {
        LOG_DEBUG("http_router_match - handler found for [%s %.*s]", http_method_to_cstr(method), path.length, path.data);
        return curr;
    }

    LOG_DEBUG("http_router_match - no handler for [%s %.*s]", http_method_to_cstr(method), path.length, path.data);
    return 0;
}

static http_router_node_t* http_router_node_maybe_create(http_router_node_t* target_node) {
    if (target_node) {
        return target_node;
    }
    
    http_router_node_t* node = memory_allocate(sizeof(http_router_node_t), MEMORY_TAG_TRIE_NODE);
    ASSERT_MSG(node, "http_router_node_maybe_create - failed node memory allocation");
    memory_zero(node, sizeof(http_router_node_t));
    for (u64 i = 0; i < _HTTP_METHOD_MAX; i++) {
        node->method_handlers[i] = (http_method_handler_t){0};
    }
    for (u64 i = 0; i < HTTP_ROUTER_CHILDREN_MAX_CAPACITY; i++) {
        node->children[i] = 0;
    }
    node->segment = STR_NULL;
    node->is_param = false;
    node->is_wildcard = false;
    return node;
}

static b8 http_router_add_route(
    http_router_t* router, 
    http_method_t method, 
    str_t path, 
    http_handler_t handler, 
    http_middleware_container_t* middleware_containers
) {
    LOG_DEBUG("http_router_add_route - adding [%s %.*s] -> %p", 
                http_method_to_cstr(method), path.length, path.data, handler);
    http_router_node_t* curr = &router->root;
    curr = http_router_node_maybe_create(curr);

    str_t rest = path;
    if (str_has_prefix_char(rest, '/')) {
        rest = str_cut_prefix(rest, str_from_cstr("/"));
    }

    while(!str_is_empty(rest)) {
        str_t segment = str_pop_first_split_char(&rest, '/');
        b8 is_param = str_has_prefix_char(segment, ':');
        b8 is_wildcard = str_compare(segment, str_from_cstr("*"));
        LOG_DEBUG("http_router_add_route - is_wildcard: %d, is_param: %d, segment: '%.*s'", 
                    is_wildcard, is_param, segment.length, segment.data);

        b8 is_found = false;

        for (u64 i = 0; i < HTTP_ROUTER_CHILDREN_MAX_CAPACITY; i++) {
            if (!curr->children[i]) {
                break;
            }
            if (str_compare(curr->children[i]->segment, segment) ||
                (is_param && curr->children[i]->is_param) ||
                (is_wildcard && curr->children[i]->is_wildcard)) {
                curr = curr->children[i];
                is_found = true;
                break;
            }
        }

        if (!is_found) {
            for (u64 i = 0; i < HTTP_ROUTER_CHILDREN_MAX_CAPACITY; i++) {
                if (!curr->children[i]) {
                    curr->children[i] = http_router_node_maybe_create(curr->children[i]);
                    curr = curr->children[i];
                    curr->segment = segment;
                    curr->is_param = is_param;
                    curr->is_wildcard = is_wildcard;
                    is_found = true;
                    break;
                }
            }
        }

        if (!is_found) {
            LOG_FATAL("http_router_add_router - could not find or create node for segment: %.*s", segment.length, segment.data);
            return false;
        }
    }

    ASSERT_MSG(curr, "http_router_add_route - internal non 0 curr node MUST be there");

    if (curr->method_handlers[method].handler != 0) {
        LOG_FATAL("http_router_add_route - trying to overwrite an existing handler with [%s %.*s] -> %p", 
                http_method_to_cstr(method), path.length, path.data, handler);
    }

    curr->method_handlers[method].handler = handler;
    curr->method_handlers[method].middleware_containers = middleware_containers;

    LOG_DEBUG("http_router_add_route - added [%s %.*s] -> %p", 
            http_method_to_cstr(method), path.length, path.data, handler);

    return true;
}
