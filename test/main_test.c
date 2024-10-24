#include <core/logger.h>

#include "core/arena_test.h"
#include "core/hashmap_test.h"
#include "core/strhashmap_test.h"
#include "core/string_test.h"
#include "core/str_test.h"
/*#include "net/http/http_test.h"*/
#include "net/http/request_test.h"
#include "net/http/router_test.h"
#include "net/url_test.h"

#include "test_manager.h"

int main() {
    test_manager_init();

    arena__register_test();
    string__register_test();
    str__register_test();
    hashmap__register_test();
    strhashmap__register_test();

    http_request__register_test();
    http_router__register_test();
    url__register_test();

    LOG_INFO("Starting tests...");

    test_manager_run();

    return 0;
}
