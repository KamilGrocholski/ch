#include "array_test.h"

#include <core/defines.h>
#include <core/array.h>

#include "../expect.h"
#include "../test_manager.h"

u8 array__should_append(void) {
    u64* values = array_create(0, u64);
    u64 expected_length = 5;
    for (u64 i = 0; i < expected_length; i++) {
        array_append(values, i);
    }

    u64 length = array_length(values);

    expect_eq(expected_length, length);

    for (u64 i = 0; i < length; i++) {
        expect_eq(i, values[i]);
    }

    array_destroy(values);

    return true;
}

void array__register_test(void) {
    test_manager_register(array__should_append, "Array should append");
}
