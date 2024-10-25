#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/string.h"
#include "core/array.h"

static b8 http_router_path_parse_for_insert(str_t path, param_entry_t** out_params) {
    ASSERT_MSG(!str_is_null(path) && !str_is_empty(path), "http_router_path_parse - path is 0 or empty");
    ASSERT_MSG(out_params, "http_router_path_parse - out_params is 0.");

    u64 segment_index = 0;
    str_t rest = path;
    while(!str_is_empty(rest)) {
        rest = str_pop_first_split_char(&rest, '/');
        str_t segment = str_pop_first_split_char(&rest, '/');
        if (!str_is_empty(segment)) {
            b8 is_param = str_has_prefix_char(segment, ':');
            if (!is_param) {
               param_entry_t entry = (param_entry_t){
                   .segment_index = segment_index, 
                   .literal = str_cut_prefix_char(segment, ':'),
               }; 
               array_append(out_params, entry);
            }
        }
        segment_index++;
    }

    return true;
}

b8 http_router_path_parse(str_t path, str_t** out_params) {
    ASSERT_MSG(!str_is_null(path) && !str_is_empty(path), "http_router_path_parse - path is 0 or empty");
    ASSERT_MSG(out_params, "http_router_path_parse - out_params is 0.");

    str_t rest = path;
    str_t param = str_pop_first_split_char(&rest, ':');
    do {
        param = str_pop_first_split_char(&rest, '/');
        ASSERT_MSG(!str_is_empty(param), "http_router_path_parse - a param within the path MUST not be empty");
        array_append(*out_params, param);
        param = str_pop_first_split_char(&rest, ':');
    } while(!str_is_empty(rest));

    return true;
}

void http_router_init(http_router_t* router) {
    router->root = trie_node_create();
    ASSERT_MSG(router->root, "http_router_init - root is not initialized");
}

void http_router_deinit(http_router_t* router) {
    if (!router) {
        LOG_ERROR("http_router_deinit - called with router 0.");
        return;
    }
    if (!router->root) {
        LOG_ERROR("http_router_deinit - router root 0.");
        return;
    }
    trie_node_destroy_all_nodes(router->root);
    router->root = 0;
}

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t key) {
    return trie_search(router->root, method, key);
}

void http_router_add(http_router_t* router, http_method_t method, const char* key, http_handler_t handler) {
    trie_insert(router->root, method, key, handler);
}

trie_node_t* trie_node_create() {
    trie_node_t* trie = memory_allocate(sizeof(trie_node_t), MEMORY_TAG_TRIE_NODE);
    ASSERT_MSG(trie, "trie_node_create - could not allocated trie.");
    memory_zero(trie, sizeof(trie_node_t));
    trie->params = array_create(0, param_entry_t);
    return trie;
}

void trie_node_destroy_all_nodes(trie_node_t* root) {
    if (!root) {
        return;
    }
    for (u16 i = 0; i < 128; i++) {
        if (root->children[i]) {
            trie_node_destroy_all_nodes(root->children[i]);
        }
    }
    array_destroy(root->params);
    memory_free(root, sizeof(trie_node_t), MEMORY_TAG_TRIE_NODE);
}

static b8 _parse_path(str_t path, str_t* out_params) {
    ASSERT_MSG(!str_is_null(path) && !str_is_empty(path), "_parse_path - path is null or empty");
    ASSERT_MSG(out_params, "_parse_path - out_params is 0.");
    str_t param;
    do {
        str_pop_first_split_char(&path, ':');
        param = str_pop_first_split_char(&path, '/');
        if (!str_is_null(param) && !str_is_empty(param)) {
            array_append(out_params, param);
        }
    } while(!str_is_empty(path));
}

void trie_insert(trie_node_t* root, http_method_t method, const char* key, http_handler_t handler) {
    ASSERT_MSG(root, "trie_insert - called with root 0.");
    ASSERT_MSG(key, "trie_insert - called with key 0.");
    ASSERT_MSG(handler, "trie_insert - called with handler 0.");

    trie_node_t* curr = root;
    while (*key) {
        if (!curr->children[(u8)*key]) {
            curr->children[(u8)*key] = trie_node_create();
        } 
        curr = curr->children[(u8)*key++];
    }

    ASSERT_MSG(curr, "trie_insert - internal curr MUST be there.");
    if (curr->handlers[method] != 0) {
        LOG_FATAL("trie_insert - trying to overwrite handler [%s %s].", http_method_to_cstr(method), key);
        return;
    }
    curr->handlers[method] = handler;
}

http_handler_t trie_search(trie_node_t* root, http_method_t method, str_t key) {
    trie_node_t* curr = root;
    for (u32 i = 0; i < key.length; i++) {
        if (!curr->children[(u8)key.data[i]]) {
            return 0;
        }
        curr = curr->children[(u8)key.data[i]];
    }
    if (!curr) {
        return 0;
    }
    http_handler_t handler = curr->handlers[method];
    if (!handler) {
        return 0;
    }
    return handler;
}
