#include "cookie_test.h"

#include <core/defines.h>
#include <core/string.h>
#include <core/str.h>
#include <net/http.h>

#include "../../expect.h"
#include "../../test_manager.h"

#include <string.h>

u8 http_cookies__should_be_parsed(void) {
    str_t str = str_from_cstr("name1=value1; name2=value2; name3=value3");
    cookies_t cookies = {0};
    cookies_init(0, &cookies);
    b8 ok = cookies_parse_cookie_request_list(str, &cookies);
    expect_true(ok);
    str_t got;
    ok = cookies_get(&cookies, str_from_cstr("name1"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "value1");
    ok = cookies_get(&cookies, str_from_cstr("name2"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "value2");
    ok = cookies_get(&cookies, str_from_cstr("name3"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "value3");
    return true;
}

u8 http_cookies__should_to_string(void) {
    cookies_t cookies = {0};
    cookies_init(0, &cookies);

    b8 ok = cookies_set(&cookies, str_from_cstr("name1"), str_from_cstr("value1"));
    expect_true(ok);

    string_t string = cookies_to_string(0, &cookies);
    expect_str_eq_cstr(str_from_cstr("name1=value1"), string);
    expect_neq(0, string);

    string_destroy(string);

    return true;
}

void http_cookie__register_test(void) {
    test_manager_register(http_cookies__should_be_parsed, "Http request cookie should be parsed");
    test_manager_register(http_cookies__should_to_string, "Http request cookie should to string");
}
