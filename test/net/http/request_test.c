#include "request_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <net/http/request.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <string.h>

#define expect_str_eq_cstr(str, cstr) do { \
    u32 cstr_length = strlen(cstr); \
    expect_neq(0, str.data); \
    expect_eq(cstr_length, cstr_length); \
    for (u32 i = 0; i < cstr_length; i++) { \
        expect_eq(cstr[i], str.data[i]); \
    } \
} while(0);

u8 http_request__should_be_parsed(void) {
    http_request_t request;
    http_request_init(0, &request);
    const char* method = "GET";
    const char* path = "/";
    const char* proto = "HTTP1.1";
    const char* header_accept_key = "Accept";
    const char* header_accept_value = "*/*";
    const char* header_host_key = "Host";
    const char* header_host_value = "cos.com";
    const char* body = "body";
    string_t raw_request = string_create(0, "");
    raw_request = string_append_format(raw_request, "%s / %s\n", method, proto);
    raw_request = string_append_format(raw_request, "%s: %s\n", header_accept_key, header_accept_value);
    raw_request = string_append_format(raw_request, "%s: %s\n", header_host_key, header_host_value);
    raw_request = string_append_format(raw_request, "\n%s", body);
    b8 ok = http_request_parse(string_to_str(raw_request), &request);
    expect_eq(true, ok);
    expect_eq(request.method, HTTP_METHOD_GET);
    expect_str_eq_cstr(request.proto, proto);
    expect_str_eq_cstr(request.body, body);
    str_t got;
    ok = http_request_headers_get(request.headers, str_from_cstr(header_accept_key), &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, header_accept_value);
    ok = http_request_headers_get(request.headers, str_from_cstr(header_host_key), &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, header_host_value);
    string_destroy(raw_request);
    http_request_deinit(&request);
    return true;
}

u8 http_request__should_put_and_get_header(void) {
    http_request_headers_t headers;
    http_request_headers_init(0, &headers);

    str_t key = str_from_cstr("tak");
    str_t value = str_from_cstr("nie");
    b8 ok = http_request_headers_set(&headers, key, value);
    expect_eq(true, ok);
    str_t got;
    ok = http_request_headers_get(&headers, key, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie");

    str_t key2 = str_from_cstr("tak2");
    str_t value2 = str_from_cstr("nie2");
    ok = http_request_headers_set(&headers, key2, value2);
    expect_eq(true, ok);
    ok = http_request_headers_get(&headers, key2, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie2");

    http_request_headers_deinit(&headers);
    return true;
}

void http_request__register_test(void) {
    test_manager_register(http_request__should_be_parsed, "Http request should be parsed");
    test_manager_register(http_request__should_put_and_get_header, "Http request should put and get header");
}
