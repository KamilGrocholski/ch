#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"

void http_router_init(http_router_t* router) {
    router->root = trie_node_create();
}

void http_router_deinit(http_router_t* router) {
    trie_node_destroy_all_nodes(router->root);
    router->root = 0;
}

http_handler_t http_router_search(http_router_t* router, http_method_t method, str_t key) {
    ASSERT(router);
    ASSERT(router->root);
    return trie_search(router->root, method, key);
}

void http_router_add(http_router_t* router, http_method_t method, const char* key, http_handler_t handler) {
    trie_insert(router->root, method, key, handler);
}

trie_node_t* trie_node_create() {
    trie_node_t* trie = memory_allocate(sizeof(trie_node_t), MEMORY_TAG_TRIE_NODE);
    memory_zero(trie, sizeof(trie_node_t));
    return trie;
}

void trie_node_destroy_all_nodes(trie_node_t* root) {
    if (!root) {
        return;
    }
    for (u16 i = 0; i < 128; i++) {
        if (root->children[i]) {
            trie_node_destroy_all_nodes(root->children[i]);
            root->children[i] = 0;
        }
    }
    memory_free(root, sizeof(trie_node_t), MEMORY_TAG_TRIE_NODE);
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

    ASSERT_MSG(curr, "trie_insert - internal curr should be there.");
    ASSERT_MSG(curr->handlers[method] == 0, "trie_insert - trying to overwrite method handler.");
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
