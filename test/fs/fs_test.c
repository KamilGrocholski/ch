#include "./fs_test.h"

#include <fs/fs.h>
#include <core/logger.h>

#include "../expect.h"
#include "../test_manager.h"

u8 fs__should_read_entire_text(void) {
    string_t content = fs_read_entire_text(0, "./test/fs/test.txt");
    expect_neq(0, content);
    expect_str_eq_cstr(string_to_str(content), "some text\nnext line");
    return true;
}

void fs__register_test(void) {
    test_manager_register(fs__should_read_entire_text, "Fs should read entire text");
}
