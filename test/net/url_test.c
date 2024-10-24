#include "url_test.h"

#include <core/defines.h>
#include <core/arena.h>
#include <core/string.h>
#include <net/url.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 url__should_parse(void) {
    url_t url;
    str_t raw_url = str_from_cstr("http://localhost:8080/users/2?name=john&age=20#somefragment");
    b8 ok = url_parse(raw_url, &url);
    expect_eq(true, ok);
    expect_str_eq_cstr(url.scheme, "http");
    expect_str_eq_cstr(url.host, "localhost");
    expect_str_eq_cstr(url.port, "8080");
    expect_str_eq_cstr(url.path, "users/2");
    expect_str_eq_cstr(url.raw_query, "name=john&age=20");
    expect_str_eq_cstr(url.raw_fragment, "somefragment");

    return true;
}

u8 url__should_parse_query(void) {
    url_query_t query;
    url_query_init(0, &query);

    str_t raw_query = str_from_cstr("name=john&age=20");
    b8 ok = url_query_parse(raw_query, &query);
    expect_eq(true, ok);

    str_t value;

    ok = url_query_get(&query, str_from_cstr("name"), &value);
    expect_eq(true, ok);
    expect_str_eq_cstr(value, "john");

    ok = url_query_get(&query, str_from_cstr("age"), &value);
    expect_eq(true, ok);
    expect_str_eq_cstr(value, "20");

    url_query_deinit(&query);

    return true;
}

u8 url__should_query_to_string(void) {
    url_query_t query;
    url_query_init(0, &query);

    const char* key1 = "key1";
    const char* value1 = "value1";

    const char* key2 = "key2";
    const char* value2 = "value2";
    string_t expected = string_create(0, "");
    expected = string_append_format(expected, "%s=%s", key1, value1);
    expected = string_append_format(expected, "%s=%s", key2, value2);

    url_query_set(&query, str_from_cstr(key1), str_from_cstr(value1));
    url_query_set(&query, str_from_cstr(key2), str_from_cstr(value2));

    string_t string = url_query_to_string(0, &query);
    expect_str_eq_cstr(string_to_str(expected), string);

    string_destroy(string);
    string_destroy(expected);
    url_query_deinit(&query);

    return true;
}

void url__register_test(void) {
    test_manager_register(url__should_parse, "Url should parse");
    test_manager_register(url__should_parse_query, "Url should parse query");
    test_manager_register(url__should_query_to_string, "Url should query to string");
}
