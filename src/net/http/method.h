#pragma once

#include "core/defines.h"
#include "core/str.h"

typedef enum http_method_t {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_DELETE,
    _HTTP_METHOD_MAX,
} http_method_t;

b8 http_method_from_str(str_t str, http_method_t* dest);

b8 http_method_from_cstr(const char* cstr, http_method_t* dest);

str_t http_method_to_str(http_method_t method);

const char* http_method_to_cstr(http_method_t method);
