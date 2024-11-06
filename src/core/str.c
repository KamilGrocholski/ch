#include "core/str.h"

#include "core/defines.h"
#include "core/logger.h"

#include <string.h>
#include <math.h>

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

str_t str_pop_first_split(str_t* str, str_t split_by) {
    i32 index = str_index(*str, split_by);
    if (index == -1) {
        str_t out = *str;
        *str = STR_EMPTY;
        return out;
    }
    str_t out = str_substr(*str, 0, index);
    *str = str_substr(*str, index + split_by.length, str->length); 
    return out;
}

str_t str_pop_first_split_char(str_t* str, char split_by) {
    i32 index = str_index_char(*str, split_by);
    if (index == -1) {
        str_t out = *str;
        *str = STR_EMPTY;
        return out;
    }
    str_t out = str_substr(*str, 0, index);
    *str = str_substr(*str, index + 1, str->length); 
    return out;
}

str_t str_trim_whitespace(str_t str) {
    return str_trim_whitespace_left(str_trim_whitespace_right(str));
}

str_t str_trim_whitespace_left(str_t str) {
    if (str_is_null_or_empty(str)) {
        return str;
    }
    u64 i = 0;
    while(i < str.length && isspace(str.data[i])) {
        i++;
    }
    str.data += i;
    str.length -= i;
    return str;
}

str_t str_trim_whitespace_right(str_t str) {
    if (str_is_null_or_empty(str)) {
        return str;
    }
    char* end = str.data + str.length - 1;
    while (end >= str.data && isspace(*end)) {
        end--;
    }
    str.length = end - str.data + 1;
    return str;
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

b8 str_cut(str_t str, str_t separator, str_t* before, str_t* after) {
    i32 index = str_index(str, separator);
    if (index == -1) {
        return false;
    }
    *before = (str_t){ .length = index, .data = str.data };
    *after = (str_t){ .length = str.length - separator.length - index, .data = str.data + separator.length + index };
    return true;
}

b8 str_cut_char(str_t str, char separator, str_t* before, str_t* after) {
    i32 index = str_index_char(str, separator);
    if (index == -1) {
        return false;
    }
    *before = (str_t){ .length = index + 1, .data = str.data };
    *after = (str_t){ .length = str.length - 1 - index, .data = str.data + 1 + index };
    return true;
}

b8 str_is_empty(str_t str) {
    return str.length == 0;
}

b8 str_is_null(str_t str) {
    return str.data == 0;
}

b8 str_is_null_or_empty(str_t str) {
    return str_is_null(str) || str_is_empty(str);
}

b8 str_compare(str_t a, str_t b) {
    if (a.length != b.length) {
        return false;
    }
    for (u64 i = 0; i < a.length; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

b8 str_compare_n(str_t a, str_t b, u64 n) {
    u64 length = MIN(MAX(a.length, b.length), n);
    for (u64 i = 0; i < length; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

b8 str_compare_char(str_t a, char ch, u64 index) {
    return a.length > index && a.data[index] == ch;
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
    return strncmp(str.data, prefix.data, prefix.length) == 0;
}

b8 str_has_prefix_char(str_t str, char prefix) {
    if (str.length < 1) {
        return false;
    }
    return str.data[0] == prefix;
}

b8 str_has_suffix(str_t str, str_t suffix) {
    if (str.length < suffix.length) {
        return false;
    }
    return strncmp(str.data + str.length - suffix.length, suffix.data, suffix.length) == 0;
}

b8 str_has_suffix_char(str_t str, char prefix) {
    if (str.length < 1) {
        return false;
    }
    return str.data[str.length - 1] == prefix;
}

u32 str_count_char(str_t str, char ch) {
    u32 n = 0;
    for (u32 i = 0; i < str.length; i++) {
        if (str.data[i] == ch) {
            n++;
        }
    }
    return n;
}

b8 str_to_u64(str_t str, u64* dest) {
    char c0 = str.data[0];
    u64 value = 0;
    if (c0 >= '1' && c0 <= '9') {
        value = (c0 - 48) * pow(10, str.length - 1);
    } else {
        return false;
    }
    for (u64 i = 1; i < str.length; i++) {
        char c = str.data[str.length - i];
        if (c >= '0' && c <= '9') {
            if (i >= NUM_64_MAX_DIGITS) {
                return false;
            }
            value += pow(10, i - 1) * (c - 48);
        } else {
            return false;
        }
    }
    *dest = value;
    return true;
}
