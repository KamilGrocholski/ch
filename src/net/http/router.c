#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/string.h"
#include "core/array.h"
#include "core/strhashmap.h"

static b8 http_router_add_route(http_router_t* router, http_method_t method, str_t path, http_handler_t handler);
static http_router_node_t* http_router_find_route_node(http_router_t* router, http_method_t method, str_t path, strhashmap_t *out_params);

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

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t path, strhashmap_t* out_params) {
    http_router_node_t* node = http_router_find_route_node(router, method, path, out_params);
    if (!node) {
        return 0;
    }
    return node->handlers[method];
}

void http_router_add(http_router_t* router, http_method_t method, const char* path, http_handler_t handler) {
    ASSERT(handler);
    ASSERT(path);
    ASSERT(router);
    ASSERT(http_router_add_route(router, method, str_from_cstr(path), handler));
}

static http_router_node_t* http_router_find_route_node(http_router_t* router, http_method_t method, str_t path, strhashmap_t *out_params) {
    http_router_node_t* curr = &router->root;

    u64 segment_index = 0;
    str_t rest = path;
    while (!str_is_empty(rest)) {
        str_t segment = str_pop_first_split_char(&rest, '/');
        b8 is_segment_empty = str_is_empty(segment);
        if (!is_segment_empty && segment.length != segment_index) {
            return false;
        }
        http_router_segment_t router_segment = curr->pattern.segments[segment_index];
        segment_index++;

        if (router_segment.is_wildcard) {
            if (is_segment_empty) {
                return 0;
            }
            if (!strhashmap_set(out_params, router_segment.literal, segment)) {
                return 0;
            }
            continue;
        }

        for (u64 i = 0; i < segment.length; i++) {
            curr = &(curr->children[i]);
            if (!curr) {
                return 0;
            }
        }
    }

    return curr;
}

static void upsert_children(http_router_node_t* node) {
    if (!node->children) {
        node->children = array_create_with_capacity(0, http_router_node_t, HTTP_ROUTER_NODE_MAX_CAPACITY);
        memory_zero(node->children, HTTP_ROUTER_NODE_MAX_CAPACITY * sizeof(http_router_node_t));
        ASSERT_MSG(node->children, "upsert_children - node allocation failed");
    }
}

static b8 http_router_add_route(http_router_t* router, http_method_t method, str_t path, http_handler_t handler) {
    http_router_node_t* curr = &router->root;
    upsert_children(curr);

    u64 segment_index = 0;
    str_t rest = str_cut_prefix(path, str_from_cstr("/"));
    LOG_DEBUG("%.*s", rest.length, rest.data);
    while (!str_is_empty(rest)) {
        str_t segment = str_pop_first_split_char(&rest, '/');
        b8 is_param = str_has_prefix(segment, str_from_cstr(":"));
        if (is_param) {
            b8 is_param_empty = segment.length <= 2;
            ASSERT_MSG(!is_param_empty, "http_router_path_parse_for_insert - param MUST not be empty");
            http_router_segment_t new_router_segment = (http_router_segment_t){
                .literal = str_cut_prefix(segment, str_from_cstr(":")),
                .is_wildcard = true,
            };
            curr->pattern.segments[curr->pattern.segments_length++] = new_router_segment;
        } else {
            http_router_segment_t new_router_segment = (http_router_segment_t){
                .literal = segment,
                .is_wildcard = false,
            };
             for (u64 i = 0; i < segment.length; i++) {
                 LOG_DEBUG("children length: %llu, i: %llu, ch: %c", array_length(&curr->children), i, segment.data[i]);
                 curr = &(curr->children[(u8)segment.data[i]]);
                 upsert_children(curr);
                 ASSERT_MSG(curr, "http_router_add_route - internal curr MUST be there");
                 ASSERT_MSG(&curr->children, "http_router_add_route - internal curr->children MUST be there");
             }
            curr->pattern.segments[curr->pattern.segments_length++] = new_router_segment;
        }
        if (!curr->children) {
            curr->children = array_create_with_capacity(0, http_router_node_t, HTTP_ROUTER_NODE_MAX_CAPACITY);
            ASSERT_MSG(curr->children, "http_router_add_route - children allocation failed");
        }
        segment_index++;
    }

    if (curr->handlers[method] != 0) {
        LOG_FATAL("http_router_add_route - trying to overwrite an existing handler [%s %.*s].", 
                http_method_to_cstr(method), path.length, path.data);
        return false;
    }
    curr->handlers[method] = handler;

    return true;
}
