#include "net/http.h"

#include <string.h>

typedef struct http_method_entry_t {
    const char* cstr;
    u32 length;
    http_method_t method;
} http_method_entry_t;

static const http_method_entry_t http_method_map[_HTTP_METHOD_MAX] = {
    {"GET", 3, HTTP_METHOD_GET},
    {"POST", 4, HTTP_METHOD_POST},
    {"DELETE", 6, HTTP_METHOD_DELETE},
};

b8 http_method_from_str(str_t str, http_method_t* dest) {
    for (i32 i = 0; i < _HTTP_METHOD_MAX; i++) {
        if (strncmp(http_method_map[i].cstr, str.data, http_method_map[i].length) == 0) {
            *dest = http_method_map[i].method;
            return true;
        }
    }
    return false;
}

b8 http_method_from_cstr(const char* cstr, http_method_t* dest) {
    for (i32 i = 0; i < _HTTP_METHOD_MAX; i++) {
        if (strncmp(http_method_map[i].cstr, cstr, http_method_map[i].length) == 0) {
            *dest = http_method_map[i].method;
            return true;
        }
    }
    return false;
}

str_t http_method_to_str(http_method_t method) {
    if (method < 0 || method >= _HTTP_METHOD_MAX) return STR_EMPTY;
    return str_from_parts(http_method_map[method].cstr, http_method_map[method].length);
}

const char* http_method_to_cstr(http_method_t method) {
    if (method < 0 || method >= _HTTP_METHOD_MAX) return 0;
    return http_method_map[method].cstr;
}
