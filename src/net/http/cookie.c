#include "net/http.h"

#include "core/logger.h"

b8 cookies_parse_cookie_request_list(str_t list, cookies_t* cookies) {
    str_t rest = str_trim_whitespace(list);
    while(!str_is_empty(rest)) {
        str_t cookie = str_pop_first_split(&rest, str_from_cstr("; "));
        if (str_is_empty(cookie)) {
            return false;
        }
        str_t name = str_pop_first_split_char(&cookie, '=');
        str_t value = cookie;
        if (str_is_empty(name)) {
            return false;
        }
        if (!cookies_set(cookies, name, value)) {
            return false;
        }
    }

    return true;
}

void cookies_init(cookies_t* cookies) {
    strhashmap_init(0, &cookies->strhashmap);
}

void cookies_deinit(cookies_t* cookies) {
    strhashmap_deinit(&cookies->strhashmap);
}

b8 cookies_get(cookies_t* cookies, str_t key, str_t* out_value) {
    return strhashmap_get(&cookies->strhashmap, key, out_value);
}

b8 cookies_set(cookies_t* cookies, str_t key, str_t value) {
    return strhashmap_set(&cookies->strhashmap, key, value);
}

string_t cookies_to_string(allocator_t* allocator, cookies_t* cookies) {
    if (!cookies) {
        LOG_ERROR("cookies_to_string - called with cookies 0.");
        return 0;
    }
    string_t string = string_from_parts(allocator, 0, cookies->strhashmap.length * 32);
    if (cookies->strhashmap.length == 0) {
        return string;
    }
    u64 i = 0;
    strhashmap_entry_t* curr = 0;
    do {
        curr = &(cookies->strhashmap.table[i++]);
        if (curr->is_occupied && string) {
            string = string_append_format(string, "%.*s=%.*s", curr->key.length, curr->key.data, curr->value.length, curr->value.data);
        }
    } while(i < cookies->strhashmap.capacity);
    return string;
}
