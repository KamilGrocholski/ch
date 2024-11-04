#include "./env_test.h"

#include <env/env.h>

#include "../expect.h"
#include "../test_manager.h"

u8 env__should_parse_env_file(void) {
    strhashmap_t envs = {0};
    strhashmap_init(0, &envs);
    b8 ok = env_parse_file(0, "./test/env/.env", &envs);
    expect_true(ok);

    str_t got;

    ok = strhashmap_get(&envs, str_from_cstr("PORT"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "8080");

    ok = strhashmap_get(&envs, str_from_cstr("DB_URL"), &got);
    expect_true(ok);
    expect_str_eq_cstr(got, "database://url");

    return true;
}

void env__register_test(void) {
    test_manager_register(env__should_parse_env_file, "Env should parse env file");
}
