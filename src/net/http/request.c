#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/strhashmap.h"

void http_request_init(allocator_t* allocator, http_request_t* dest) {
    *dest = (http_request_t){0};
    strhashmap_init(allocator, &dest->headers);
}

void http_request_deinit(http_request_t* request) {
    if (!request) {
        LOG_ERROR("http_request_deinit - called with request 0.");
        return;
    }
    strhashmap_deinit(&request->headers);
    strhashmap_deinit(&request->params);
}

b8 http_request_parse(str_t raw_request, http_request_t* dest) {
    ASSERT_MSG(dest, "http_request_parse - dest MUST not be 0.");

    b8 is_raw_request_null = str_is_null(raw_request);
    if (is_raw_request_null) {
        return false;
    }
    b8 is_raw_request_empty = str_is_empty(raw_request);
    if (is_raw_request_empty) {
        return false;
    }

    // Split headers and body
    str_t headers = str_pop_first_split(&raw_request, str_from_cstr("\n\n"));
    str_t body = raw_request;
    dest->body = body;

    // The first line is method, path and proto, e.g. 'GET / HTTP1.1'
    str_t header_line = str_pop_first_split_char(&headers, '\n');
    if (str_is_empty(header_line)) {
        return false;
    }
    str_t method = str_pop_first_split_char(&header_line, ' ');
    str_t path = str_pop_first_split_char(&header_line, ' ');
    str_t proto = header_line;
    if (!http_method_from_str(method, &dest->method)) {
        return false;
    }
    dest->path = path;
    dest->proto = proto;

    // Split key value pairs
    header_line = str_pop_first_split_char(&headers, '\n');
    str_t key;
    str_t value;
    while (!str_is_empty(header_line)) {
        key = str_pop_first_split(&header_line, str_from_cstr(": "));
        value = header_line;

        if (!strhashmap_set(&dest->headers, key, value)) {
            return false;
        }
        header_line = str_pop_first_split_char(&headers, '\n');
    }

    return true;
}
