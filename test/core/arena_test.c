#include "arena_test.h"

#include <core/defines.h>
#include <core/arena.h>

#include "../expect.h"
#include "../test_manager.h"

u8 arena__should_create_and_destroy(void) {
    arena_t arena = {0};
    arena_create(sizeof(u64), 0, &arena);
    expect_neq(0, arena.memory);
    expect_eq(sizeof(u64), arena.total_size);
    expect_eq(0, arena.allocated);
    expect_true(arena.owns_memory);

    arena_destroy(&arena);
    expect_eq(0, arena.memory);
    expect_eq(0, arena.total_size);
    expect_eq(0, arena.allocated);

    return true;
}

void arena__register_test(void) {
    test_manager_register(arena__should_create_and_destroy, "Arena should create and destroy");
}
