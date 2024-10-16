#include "test_manager.h"

#include <core/array.h>
#include <core/logger.h>
#include <core/string.h>

typedef struct test_entry {
    PFN_test func;
    char* desc;
} test_entry;

static test_entry* tests;

void test_manager_init(void) {
    tests = array_create(test_entry, 0);
}

void test_manager_register(u8 (*PFN_test)(void), char* desc) {
    test_entry e;
    e.func = PFN_test;
    e.desc = desc;
    array_append(tests, e);
}

void test_manager_run(void) {
    u32 passed = 0;
    u32 failed = 0;
    u32 skipped = 0;

    u32 count = array_length(tests);

    /*kclock total_time;*/
    /*kclock_start(&total_time);*/

    for (u32 i = 0; i < count; ++i) {
        /*kclock test_time;*/
        /*kclock_start(&test_time);*/
        u8 result = tests[i].func();
        /*kclock_update(&test_time);*/

        if (result == true) {
            ++passed;
        } else if (result == BYPASS) {
            LOG_WARN("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        } else {
            LOG_ERROR("[FAILED]: %s", tests[i].desc);
            ++failed;
        }
        string_t status = string_create_with_capacity(0, 20);;
        string_appendf(&status, failed ? "*** %d FAILED ***" : "SUCCESS", failed);
        /*kclock_update(&total_time);*/
        /*LOG_INFO("Executed %d of %d (skipped %d) %.*s (%.6f sec / %.6f sec total", i + 1, */
        /*        count, skipped, status.length, status.data, test_time.elapsed, total_time.elapsed);*/
        LOG_INFO("Executed %d of %d (skipped %d) %.*s (%.6f sec / %.6f sec total", i + 1, 
                count, skipped, status.length, status.data, 0.8f, 0.4f);
    }

    /*kclock_stop(&total_time);*/

    LOG_INFO("Results: %d passed, %d failed, %d skipped.", passed, failed, skipped);
}
