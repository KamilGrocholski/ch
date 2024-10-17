#include <core/logger.h>
#include <core/memory.h>

#include "core/arena_test.h"

#include "test_manager.h"

int main() {
    memory_init();
    test_manager_init();
    memory_report();

    arena__register_test();
    memory_report();

    LOG_INFO("Starting tests...");
    memory_report();

    /*KDEBUG("Starting tests...");*/

    test_manager_run();
    memory_report();
    memory_shutdown();

    return 0;
}
