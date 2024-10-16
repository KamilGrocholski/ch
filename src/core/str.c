#include "core/str.h"

#include "core/defines.h"

#include <string.h>

str_t str_from_parts(const char* data, u64 length) {
    return (str_t){
        .length = length,
        .data = data,
    };
}

str_t str_from_cstr(const char* cstr) {
    return (str_t){
        .length = strlen(cstr), 
        .data = cstr,
    };
}

str_t str_substr(str_t str, u64 start, u64 end) {
    if (start >= end || end > str.length) {
        return STR_EMPTY;
    }
    return (str_t){
        .length = end - start, 
        .data = str.data + start,
    };
}

str_t str_cut_prefix(str_t str, str_t prefix) {
    if (!str_has_prefix(str, prefix)) {
        return STR_EMPTY;
    }
    return (str_t){
        .length = str.length - prefix.length, 
        .data = str.data + prefix.length,
    };
}

str_t str_cut_suffix(str_t str, str_t suffix) {
    if (!str_has_suffix(str, suffix)) {
        return STR_EMPTY;
    }
    return (str_t){
        .length = str.length - suffix.length, 
        .data = str.data,
    };
}

i32 str_index(str_t str, str_t substr) {
    if (str.length < substr.length) {
        return -1;
    }
    for (u32 i = 0; i <= str.length - substr.length; i++) {
        if (strncmp(str.data + i, substr.data, substr.length) == 0) {
            return i;
        }
    }
    return -1;
}

i32 str_index_char(str_t str, char ch) {
    for (u32 i = 0; i < str.length; i++) {
        if (str.data[i] == ch) {
            return i;
        }
    }
    return -1;
}

b8 str_is_empty(str_t str) {
    return str.length == 0;
}

b8 str_is_null(str_t str) {
    return str.data == 0;
}

b8 str_compare(str_t a, str_t b) {
    if (a.length != b.length) {
        return false;
    }
    for (u64 i = 0; i <= a.length ; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

b8 str_contains(str_t str, str_t substr) {
    return str_index(str, substr) >= 0;
}

b8 str_contains_char(str_t str, char ch) {
    return str_index_char(str, ch) >= 0;
}

b8 str_has_prefix(str_t str, str_t prefix) {
    if (str.length < prefix.length) {
        return false;
    }
    return strncmp(str.data, prefix.data, prefix.length);
}

b8 str_has_suffix(str_t str, str_t suffix) {
    if (str.length < suffix.length) {
        return false;
    }
    return strncmp(str.data + str.length - suffix.length, suffix.data, suffix.length);
}
