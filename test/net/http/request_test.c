#include "request_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <string.h>

u8 http_request__should_be_parsed(void) {
    http_request_t request = {0};
    http_request_init(0, &request);
    const char* method = "GET";
    const char* path = "/";
    const char* proto = "HTTP1.1";
    const char* header_accept_key = "Accept";
    const char* header_accept_value = "*/*";
    const char* header_host_key = "Host";
    const char* header_host_value = "cos.com";
    const char* header_cookie_key = "Cookie";
    const char* header_cookie_value = "theme=dark";
    const char* body = "body";
    string_t raw_request = string_create(0, "");
    raw_request = string_append_format(raw_request, "%s %s %s\n", method, path, proto);
    raw_request = string_append_format(raw_request, "%s: %s\n", header_accept_key, header_accept_value);
    raw_request = string_append_format(raw_request, "%s: %s\n", header_cookie_key, header_cookie_value);
    raw_request = string_append_format(raw_request, "%s: %s\n", header_host_key, header_host_value);
    raw_request = string_append_format(raw_request, "\n%s", body);
    b8 ok = http_request_parse(string_to_str(raw_request), &request);
    expect_eq(true, ok);
    expect_eq(3, request.headers.length);
    expect_eq(request.method, HTTP_METHOD_GET);
    expect_str_eq_cstr(request.path, path);
    expect_str_eq_cstr(request.proto, proto);
    expect_str_eq_cstr(request.body, body);
    str_t got;
    ok = strhashmap_get(&request.headers, str_from_cstr(header_accept_key), &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, header_accept_value);
    ok = strhashmap_get(&request.headers, str_from_cstr(header_host_key), &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, header_host_value);
    ok = cookies_get(&request.cookies, str_from_cstr("theme"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "dark");
    string_destroy(raw_request);
    http_request_deinit(&request);
    return true;
}

void http_request__register_test(void) {
    test_manager_register(http_request__should_be_parsed, "Http request should be parsed");
}
