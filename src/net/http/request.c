#include "net/http.h"

#include "core/assert.h"
#include "core/logger.h"
#include "core/strhashmap.h"
#include "core/memory.h"
#include "core/string.h"

void http_request_init(allocator_t* allocator, http_request_t* dest) {
    strhashmap_init(allocator, &dest->headers);
}

void http_request_deinit(http_request_t* request) {
    if (!request) {
        LOG_ERROR("http_request_deinit - called with request 0.");
        return;
    }
    strhashmap_deinit(&request->headers);
}

string_t http_request_to_string(allocator_t* allocator, http_request_t* request) {
    string_t string = string_create(allocator, "request:\n");
    if (!string) {
        return 0;
    }
    string = string_append_format(string, "    method: %s\n", http_method_to_cstr(request->method));
    string = string_append_format(string, "    path: %.*s\n", request->path.length, request->path.data);
    string = string_append_format(string, "    proto: %.*s\n", request->proto.length, request->proto.data);
    string = string_append_format(string, "    body: %.*s", request->body.length, request->body.data);
    return string;
}

b8 http_request_headers_get(http_request_t* request, str_t key, str_t* out_value) {
    return strhashmap_get(&request->headers, key, out_value);
}

b8 http_request_parse(str_t raw_request, http_request_t* out_request) {
    ASSERT_MSG(out_request, "http_request_parse - out_request MUST not be 0.");

    LOG_DEBUG("http_request_parse - raw_request:\n%.*s", raw_request.length, raw_request.data);

    b8 is_raw_request_null = str_is_null(raw_request);
    if (is_raw_request_null) {
        LOG_DEBUG("http_request_parse - null request");
        return false;
    }
    b8 is_raw_request_empty = str_is_empty(raw_request);
    if (is_raw_request_empty) {
        LOG_DEBUG("http_request_parse - empty request");
        return false;
    }

    // Split headers and body
    str_t headers = str_pop_first_split(&raw_request, str_from_cstr("\n\n"));
    str_t body = raw_request;
    out_request->body = body;

    // The first line is method, path and proto, e.g. 'GET / HTTP1.1'
    str_t header_line = str_pop_first_split_char(&headers, '\n');
    if (str_is_empty(header_line)) {
        LOG_DEBUG("http_request_parse - empty request line");
        return false;
    }
    str_t method = str_pop_first_split_char(&header_line, ' ');
    str_t path = str_pop_first_split_char(&header_line, ' ');
    str_t proto = header_line;
    if (!http_method_from_str(method, &out_request->method)) {
        LOG_DEBUG("http_request_parse - invalid method");
        return false;
    }
    out_request->path = path;
    out_request->proto = proto;

    // Split key value pairs
    while (!str_is_empty(headers)) {
        header_line = str_pop_first_split_char(&headers, '\n');
        LOG_DEBUG("http_request_parse - header line: %.*s", header_line.length, header_line.data);
        str_t key = str_pop_first_split(&header_line, str_from_cstr(": "));
        str_t value = header_line;

        if (str_is_null_or_empty(key)) {
            LOG_DEBUG("http_request_parse - null or empty key");
            return false;
        }
        if (!strhashmap_set(&out_request->headers, key, value)) {
            LOG_DEBUG("http_request_parse - could not set header");
            return false;
        }
        LOG_DEBUG("http_request_parse - header kv: [%.*s]: '%.*s'", key.length, key.data, value.length, value.data);
    }

    return true;
}
