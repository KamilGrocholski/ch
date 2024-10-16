#include <core/logger.h>

#include "core/arena_test.h"

#include "test_manager.h"

int main() {
    test_manager_init();

    arena__register_test();

    LOG_INFO("Starting tests...");

    /*KDEBUG("Starting tests...");*/

    test_manager_run();

    return 0;
}
