#include "mime_test.h"

#include <core/defines.h>
#include <core/arena.h>
#include <core/str.h>
#include <net/mime.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 mime__should_from_str(void) {
    str_t literal = str_from_cstr("application/javascript");
    mime_t mime;
    expect_true(mime_from_str(literal, &mime));
    expect_eq(MIME_APPLICATION_JAVASCRIPT, mime);
    return true;
}

u8 mime__should_to_str(void) {
    str_t literal;
    mime_t mime = MIME_APPLICATION_JAVASCRIPT;
    expect_true(mime_to_str(mime, &literal));
    expect_str_eq_cstr(literal, "application/javascript");
    return true;
}

void mime__register_test(void) {
    test_manager_register(mime__should_from_str, "MIME should from str");
    test_manager_register(mime__should_to_str, "MIME should to str");
}
