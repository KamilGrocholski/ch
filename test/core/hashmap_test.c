#include "hashmap_test.h"

#include <core/defines.h>
#include <core/hashmap.h>

#include "../expect.h"
#include "../test_manager.h"

#include <string.h>

u8 hashmap__should_create_and_destroy(void) {
    hashmap_t* hashmap = hashmap_create(0, sizeof(char*));
    expect_neq(0, hashmap);
    string_destroy(hashmap);
    return true;
}

u8 hashmap__should_put_and_get(void) {
    hashmap_t* hashmap = hashmap_create(0, sizeof(char*));
    const char* value = "value";
    expect_eq(true, hashmap_put(hashmap, "key", value));
    char* got;
    b8 ok = hashmap_get(hashmap, "key", &got);
    expect_eq(true, ok);
    expect_eq(true, strncmp(got, "value", 5) == 0);
    string_destroy(hashmap);
    return true;
}

u8 hashmap__should_resize(void) {
    hashmap_t* hashmap = hashmap_create(0, sizeof(u64*));
    for (u64 i = 0; i < HASHMAP_DEFAULT_CAPACITY * 2; i++) {
        expect_eq(true, hashmap_put(hashmap, "key", &i));
        u64 got;
        b8 ok = hashmap_get(hashmap, "key", &got);
        expect_eq(true, ok);
        expect_eq(i, i);
    }
    string_destroy(hashmap);
    return true;
}

u8 hashmap__should_put_and_remove(void) {
    hashmap_t* hashmap = hashmap_create(0, sizeof(char*));
    expect_eq(true, hashmap_put(hashmap, "key", "value"));
    b8 ok = hashmap_remove(hashmap, "key");
    expect_eq(true, ok);
    string_destroy(hashmap);
    return true;
}

u8 hashmap__should_not_crash_on_null_destroy(void) {
    string_destroy(0);
    return true;
}

void hashmap__register_test(void) {
    test_manager_register(hashmap__should_create_and_destroy, "Hashmap should create and destroy");
    test_manager_register(hashmap__should_resize, "Hashmap should resize");
    test_manager_register(hashmap__should_put_and_get, "Hashmap should put and get");
    test_manager_register(hashmap__should_put_and_remove, "Hashmap should put and remove");
    test_manager_register(hashmap__should_not_crash_on_null_destroy, "Hashmap should not crash on null destroy");
}
