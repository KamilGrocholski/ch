#include "./unicode_test.h"

#include <unicode/unicode.h>
#include <core/logger.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 unicode_utf8__should_encode(void) {
    char buffer[5];

    struct {
        u32 length;
        u32 codepoint;
        const char* utf8;
    } test_cases[] = {
        {1, 0x24, "$"},
        {2, 0xa2, "\xc2\xa2"},
        {3, 0x20ac, "\xe2\x82\xac"},
        {4, 0x1f600, "\xf0\x9f\x98\x80"},
    };

    i32 cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
    for (i32 i = 0; i < cases_length; i++) {
        utf8_encode(test_cases[i].codepoint, buffer, sizeof(buffer));
        
        i32 e_cmp = memcmp(buffer, test_cases[i].utf8, test_cases[i].length);
        expect_eq(0, e_cmp);
    }
    
    return true;
}

u8 unicode_utf8__should_decode(void) {
    u32 decoded_codepoint;
    u32 length;

    struct {
        const char* utf8;
        u32 length;
        u32 codepoint;
    } test_cases[] = {
        {"$", 1, 0x24},
        {"\xc2\xa2", 2, 0xa2},
        {"\xe2\x82\xac", 3, 0x20ac},
        {"\xf0\x9f\x98\x80", 4, 0x1f600},
    };

    i32 cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
    for (i32 i = 0; i < cases_length; i++) {
        length = utf8_decode(test_cases[i].utf8, strlen(test_cases[i].utf8), &decoded_codepoint);
        
        expect_eq(decoded_codepoint, test_cases[i].codepoint);
        expect_eq(length, test_cases[i].length);
    }

    return true;
}

void unicode__register_test(void) {
    test_manager_register(unicode_utf8__should_encode, "Unicode UTF-8 should encode");
    test_manager_register(unicode_utf8__should_decode, "Unicode UTF-8 should decode");
}

