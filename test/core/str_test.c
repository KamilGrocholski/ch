#include "str_test.h"

#include <core/defines.h>
#include <core/str.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 str__should_create_from_cstr(void) {
    const char* expected_cstr = "siema";
    str_t str = str_from_cstr(expected_cstr);
    expect_str_eq_cstr(str, expected_cstr);
    return true;
}

u8 str__should_be_empty(void) {
    str_t str = str_from_cstr("");
    expect_eq(true, str_is_empty(str));
    return true;
}

u8 str__should_not_be_empty(void) {
    str_t str = str_from_cstr("a");
    expect_eq(false, str_is_empty(str));
    return true;
}

u8 str__should_be_null(void) {
    str_t str; 
    expect_eq(true, str_is_null(str));
    return true;
}

u8 str__should_not_be_null(void) {
    str_t str = str_from_cstr("");
    expect_eq(false, str_is_null(str));
    return true;
}

u8 str__should_have_prefix(void) {
    str_t str = str_from_cstr("ABCDefg");
    str_t prefix = str_from_cstr("ABCD");
    expect_eq(true, str_has_prefix(str, prefix));
    return true;
}

u8 str__should_have_suffix(void) {
    str_t str = str_from_cstr("ABCDefg");
    str_t suffix = str_from_cstr("efg");
    expect_eq(true, str_has_suffix(str, suffix));
    return true;
}

u8 str__should_cut_prefix(void) {
    str_t str = str_from_cstr("ABCDefg");
    str_t prefix = str_from_cstr("ABCD");
    str_t got = str_cut_prefix(str, prefix);
    const char* expected = "efg";
    expect_str_eq_cstr(got, expected);
    return true;
}

u8 str__should_cut_suffix(void) {
    str_t str = str_from_cstr("ABCDefg");
    str_t suffix = str_from_cstr("efg");
    str_t got = str_cut_suffix(str, suffix);
    const char* expected = "ABCD";
    expect_str_eq_cstr(got, expected);
    return true;
}

u8 str__should_substr(void) {
    str_t str = str_from_cstr("aABCDefg");
    str_t got = str_substr(str, 1, 5);
    const char* expected = "ABCD";
    expect_str_eq_cstr(got, expected);
    return true;
}

u8 str__should_index(void) {
    str_t str = str_from_cstr("aABCDefg");
    i32 got = str_index(str, str_from_cstr("ABCD"));
    expect_eq(1, got);
    return true;
}

u8 str__should_not_index(void) {
    str_t str = str_from_cstr("aABCDefg");
    i32 got = str_index(str, str_from_cstr("ABED"));
    expect_eq(-1, got);
    return true;
}

u8 str__should_index_char(void) {
    str_t str = str_from_cstr("aABCDefg");
    i32 got = str_index_char(str, 'A');
    expect_eq(1, got);
    return true;
}

u8 str__should_not_index_char(void) {
    str_t str = str_from_cstr("aABCDefg");
    i32 got = str_index_char(str, 'x');
    expect_eq(-1, got);
    return true;
}

u8 str__should_cut(void) {
    str_t str = str_from_cstr("ABCD123efg");
    str_t before;
    str_t after;
    str_t separator = str_from_cstr("123");
    b8 ok = str_cut(str, separator, &before, &after);
    expect_eq(true, ok);
    expect_str_eq_cstr(before, "ABCD");
    expect_str_eq_cstr(after, "efg");
    return true;
}

u8 str__should_not_cut(void) {
    str_t str = str_from_cstr("ABCD12efg");
    str_t before;
    str_t after;
    str_t separator = str_from_cstr("123");
    b8 ok = str_cut(str, separator, &before, &after);
    expect_eq(false, ok);
    return true;
}

u8 str__should_cut_char(void) {
    str_t str = str_from_cstr("ABCD0efg");
    str_t before;
    str_t after;
    char separator = '0';
    b8 ok = str_cut_char(str, separator, &before, &after);
    expect_eq(true, ok);
    expect_str_eq_cstr(before, "ABCD");
    expect_str_eq_cstr(after, "efg");
    return true;
}

u8 str__should_be_equal(void) {
    str_t str_a = str_from_cstr("ABCD");
    str_t str_b = str_from_cstr("ABCD");
    expect_eq(true, str_compare(str_a, str_b));
    return true;
}

u8 str__should_not_be_equal(void) {
    str_t str_a = str_from_cstr("ABCD");
    str_t str_b = str_from_cstr("ABCb");
    expect_eq(false, str_compare(str_a, str_b));
    return true;
}

u8 str__should_contain(void) {
    str_t str = str_from_cstr("ABCD123efg");
    str_t substr = str_from_cstr("123");
    expect_eq(true, str_contains(str, substr));
    return true;
}

u8 str__should_not_contain(void) {
    str_t str = str_from_cstr("ABCD123efg");
    str_t substr = str_from_cstr("1234");
    expect_eq(false, str_contains(str, substr));
    return true;
}

u8 str__should_pop_first_split(void) {
    str_t str = str_from_cstr("key1::value1..key2::value2..key3::value3");
    str_t split_by = str_from_cstr("..");
    str_t got;

    got = str_pop_first_split(&str, split_by);
    expect_str_eq_cstr(got, "key1::value1");
    expect_eq(false, str_is_null(got))
    expect_eq(false, str_is_empty(got))

    got = str_pop_first_split(&str, split_by);
    expect_str_eq_cstr(got, "key2::value2");
    expect_eq(false, str_is_null(got))
    expect_eq(false, str_is_empty(got))

    got = str_pop_first_split(&str, split_by);
    expect_str_eq_cstr(got, "key3::value3");
    expect_eq(false, str_is_null(got))
    expect_eq(false, str_is_empty(got))

    expect_str_eq_cstr(str, "");

    return true;
}

void str__register_test(void) {
    test_manager_register(str__should_create_from_cstr, "Str should create from cstr");
    test_manager_register(str__should_be_empty, "Str should be empty");
    test_manager_register(str__should_not_be_empty, "Str should not be empty");
    test_manager_register(str__should_be_null, "Str should be null");
    test_manager_register(str__should_not_be_null, "Str should not be null");
    test_manager_register(str__should_have_prefix, "Str should have prefix");
    test_manager_register(str__should_have_suffix, "Str should have suffix");
    test_manager_register(str__should_cut_prefix, "Str should cut prefix");
    test_manager_register(str__should_cut_suffix, "Str should cut suffix");
    test_manager_register(str__should_substr, "Str should substr");
    test_manager_register(str__should_index, "Str should index");
    test_manager_register(str__should_not_index, "Str should not index");
    test_manager_register(str__should_index_char, "Str should index char");
    test_manager_register(str__should_not_index_char, "Str should not index char");
    test_manager_register(str__should_cut, "Str should cut");
    test_manager_register(str__should_not_cut, "Str should not cut");
    test_manager_register(str__should_cut_char, "Str should cut char");
    test_manager_register(str__should_be_equal, "Str should be equal");
    test_manager_register(str__should_not_be_equal, "Str should not be equal");
    test_manager_register(str__should_contain, "Str should contain");
    test_manager_register(str__should_not_contain, "Str should not contain");
    test_manager_register(str__should_pop_first_split, "Str should pop first split");
}
