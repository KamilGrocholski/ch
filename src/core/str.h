#pragma once

#include "core/defines.h"

typedef struct str_t {
    u64 length;
    const char* data;
} str_t;

#define STR_FMT "%.*s"

#define STR_EMPTY (str_t){.length = 0, .data = ""}

#define STR_NULL (str_t){.length = 0, .data = 0}

str_t str_from_parts(const char* data, u64 length);

str_t str_from_cstr(const char* cstr);

str_t str_substr(str_t str, u64 start, u64 end);

str_t str_cut_prefix(str_t str, str_t prefix);

str_t str_cut_suffix(str_t str, str_t suffix);

str_t str_pop_first_split(str_t* str, str_t split_by);

str_t str_pop_first_split_char(str_t* str, char split_by);

str_t str_trim_whitespace(str_t str);

str_t str_trim_whitespace_left(str_t str);

str_t str_trim_whitespace_right(str_t str);

i32 str_index(str_t str, str_t substr);

i32 str_index_char(str_t str, char ch);

b8 str_cut(str_t str, str_t separator, str_t* before, str_t* after);

b8 str_cut_char(str_t str, char separator, str_t* before, str_t* after);

b8 str_is_empty(str_t str);

b8 str_is_null(str_t str);

b8 str_is_null_or_empty(str_t str);

b8 str_compare(str_t a, str_t b);

b8 str_compare_n(str_t a, str_t b, u64 n);

b8 str_compare_ci(str_t a, str_t b);

b8 str_compare_n_ci(str_t a, str_t b, u64 n);

b8 str_compare_char(str_t a, char ch, u64 index);

b8 str_contains(str_t str, str_t substr);

b8 str_contains_char(str_t str, char ch);

b8 str_has_prefix(str_t str, str_t prefix);

b8 str_has_prefix_char(str_t str, char prefix);

b8 str_has_suffix(str_t str, str_t suffix);

b8 str_has_suffix_char(str_t str, char suffix);

u32 str_count_char(str_t str, char ch);

b8 str_to_u64(str_t str, u64* dest);
