#include "hashmap_test.h"

#include <core/defines.h>
#include <core/strhashmap.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 strhashmap__should_set_and_get(void) {
    strhashmap_t hashmap;
    strhashmap_init(0, &hashmap);

    str_t key = str_from_cstr("tak");
    str_t value = str_from_cstr("nie");
    b8 ok = strhashmap_set(&hashmap, key, value);
    expect_eq(true, ok);
    str_t got;
    ok = strhashmap_get(&hashmap, key, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie");

    str_t key2 = str_from_cstr("tak2");
    str_t value2 = str_from_cstr("nie2");
    ok = strhashmap_set(&hashmap, key2, value2);
    expect_eq(true, ok);
    ok = strhashmap_get(&hashmap, key2, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie2");

    strhashmap_deinit(&hashmap);
    return true;
}

u8 strhashmap__should_set_and_get_ci(void) {
    strhashmap_t hashmap;
    strhashmap_init(0, &hashmap);

    str_t key_uppercase = str_from_cstr("TAK");
    str_t key = str_from_cstr("tak");
    str_t value = str_from_cstr("nie");
    b8 ok = strhashmap_set(&hashmap, key, value);
    expect_eq(true, ok);
    str_t got;
    ok = strhashmap_get(&hashmap, key_uppercase, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie");

    str_t key2 = str_from_cstr("tak2");
    str_t key2_uppercase = str_from_cstr("TAK2");
    str_t value2 = str_from_cstr("nie2");
    ok = strhashmap_set(&hashmap, key2, value2);
    expect_eq(true, ok);
    ok = strhashmap_get_ci(&hashmap, key2_uppercase, &got);
    expect_eq(true, ok);
    expect_str_eq_cstr(got, "nie2");

    strhashmap_deinit(&hashmap);
    return true;
}

void strhashmap__register_test(void) {
    test_manager_register(strhashmap__should_set_and_get, "Strhashmap should create and destroy");
}
