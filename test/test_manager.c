#include "test_manager.h"

#include <core/array.h>
#include <core/logger.h>
#include <core/string.h>

typedef struct test_entry_t {
    PFN_test func;
    char* desc;
} test_entry_t;

static test_entry_t* tests;

void test_manager_init(void) {
    tests = array_create(0, test_entry_t);
}

void test_manager_register(u8 (*PFN_test)(void), char* desc) {
    test_entry_t e;
    e.func = PFN_test;
    e.desc = desc;
    array_append(tests, e);
}

void test_manager_run(void) {
    u32 passed = 0;
    u32 failed = 0;
    u32 skipped = 0;

    u32 count = array_length(tests);

    for (u32 i = 0; i < count; ++i) {
        u8 result = tests[i].func();

        if (result == true) {
            ++passed;
        } else if (result == BYPASS) {
            LOG_WARN("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        } else {
            LOG_ERROR("[FAILED]: %s", tests[i].desc);
            ++failed;
        }
        string_t status = string_from_parts(0, 0, 128);
        status = string_append_format(status, failed ? "*** %d FAILED ***" : "SUCCESS", failed);
        LOG_INFO("Executed %d of %d (skipped %d) %s", i + 1, count, skipped, status);
        string_destroy(status);
    }

    LOG_INFO("Results: %d passed, %d failed, %d skipped.", passed, failed, skipped);
}
