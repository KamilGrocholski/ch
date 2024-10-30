#include "string_test.h"

#include <core/defines.h>
#include <core/string.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 string__should_create_and_destroy(void) {
    string_t string = string_create(0, "siema");
    expect_neq(0, string);
    expect_eq('\0', string[strlen(string)]);
    expect_eq(strlen(string), string_length(string));

    string_destroy(string);

    return true;
}

u8 string__should_create_from_parts(void) {
    const char* expected_cstr = "siema";
    u32 expected_length = strlen(expected_cstr);
    string_t string = string_from_parts(0,expected_cstr, expected_length);
    expect_neq(0, string);
    expect_eq('\0', string[strlen(string)]);
    expect_eq(expected_length, string_length(string));
    for (u32 i = 0; i < expected_length; i++) {
       expect_eq(expected_cstr[i], string[i]); 
    }

    string_destroy(string);

    return true;
}

u8 string__should_append_string(void) {
    const char* seg1 = "seg1";
    u32 seg1_length = strlen(seg1);
    const char* seg2 = "seg1";
    u32 seg2_length = strlen(seg2);
    string_t string = string_create(0, seg1);
    expect_neq(0, string);
    string_t other = string_create(0, seg2);
    expect_neq(0, other);

    char expected_cstr[seg1_length + seg2_length + 1];
    sprintf(expected_cstr, "%s%s", seg1, seg2);
    u32 expected_length = strlen(expected_cstr);

    string = string_append_string(string, other);
    expect_eq(expected_length, strlen(string));
    expect_eq('\0', string[strlen(string)]);
    expect_eq(expected_length, string_length(string));
    for (u32 i = 0; i < expected_length; i++) {
       expect_eq(expected_cstr[i], string[i]); 
    }

    string_destroy(string);
    string_destroy(other);

    return true;
}

u8 string__should_append_cstr(void) {
    string_t string = string_create(0, "");
    expect_neq(0, string);

    const char* expected_cstr = "siema";
    u32 expected_length = strlen(expected_cstr);
    string = string_append_cstr(string, expected_cstr);
    expect_eq(expected_length, strlen(string));
    expect_eq('\0', string[strlen(string)]);
    expect_eq(expected_length, string_length(string));
    for (u32 i = 0; i < expected_length; i++) {
       expect_eq(expected_cstr[i], string[i]); 
    }

    string_destroy(string);

    return true;
}

u8 string__should_append_format(void) {
    string_t string = string_create(0, "");
    expect_neq(0, string);

    const char* format = "%.*s: %d, %s, %.*s";
    str_t seg1 = str_from_cstr("seg11.1");
    int seg2 = 100;
    const char* seg3 = "value";
    str_t seg4 = str_from_cstr("httpsmth");
    char expected_formatted[2000];
    sprintf(expected_formatted, format, seg1.length, seg1.data, seg2, seg3, seg4.length, seg4.data);
    u32 expected_length = strlen(expected_formatted);

    string = string_append_format(string, format, seg1.length, seg1.data, seg2, seg3, seg4.length, seg4.data);
    expect_eq(expected_length, strlen(string));
    expect_eq('\0', string[strlen(string)]);
    expect_eq(expected_length, string_length(string));
    for (u32 i = 0; i < expected_length; i++) {
       expect_eq(expected_formatted[i], string[i]); 
    }

    string_destroy(string);

    return true;
}

u8 string__should_not_crash_on_null_string_destroy(void) {
    string_destroy(0);
    return true;
}

void string__register_test(void) {
    test_manager_register(string__should_create_and_destroy, "String should create and destroy");
    test_manager_register(string__should_create_from_parts, "String should create from parts");
    test_manager_register(string__should_append_string, "String should append string");
    test_manager_register(string__should_append_cstr, "String should append cstr");
    test_manager_register(string__should_append_format, "String should append format");
    test_manager_register(string__should_not_crash_on_null_string_destroy, "String should not crash on null string destroy");
}
