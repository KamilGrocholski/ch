#include "net/http.h"

#include "core/logger.h"

string_t cookie_to_string(allocator_t* allocator, cookie_t cookie) {
    string_t string = string_create(allocator, "");
    string = string_append_format(string, "%.*s=%.*s", cookie.name.length, cookie.name.data, cookie.value.length, cookie.value.data);
    string = string_append_format(string, "; Path=%.*s", cookie.path.length, cookie.path.data);
    string = string_append_format(string, "; Domain=%.*s", cookie.domain.length, cookie.domain.data);
    string = string_append_format(string, "; Expires=%.*s", cookie.raw_expires.length, cookie.raw_expires.data);
    // string = string_append_format(string, "; Max-Age=%.*s", cookie.path.length, cookie.path.data);
    switch(cookie.same_site) {
        case COOKIE_SAME_SITE_LAX_MODE:
            string = string_append_format(string, "; SameSite=Lax");
            break;
        case COOKIE_SAME_SITE_STRICT_MODE:
            string = string_append_format(string, "; SameSite=Strict");
            break;
        case COOKIE_SAME_SITE_NONE_MODE:
            string = string_append_format(string, "; SameSite=None");
            break;
        default:
            string = string_append_format(string, "; SameSite=None");
            break;
    }
    if (cookie.secure) {
        string = string_append_format(string, "; Secure");
    }
    if (cookie.http_only) {
        string = string_append_format(string, "; HttpOnly");
    }
    return string;
}

b8 set_cookies_init(allocator_t* allocator, set_cookies_t* cookies) {
    string_t writer = string_create(allocator, "");
    if (!writer) {
        return false;
    }
    cookies->writer = writer;
    return true;
}

void set_cookies_deinit(set_cookies_t* cookies) {
    string_destroy(cookies->writer);
}

b8 set_cookies_write(set_cookies_t* cookies, cookie_t cookie) {
    string_t cookie_string = cookie_to_string(0, cookie);
    if (!cookie_string) {
        return false;
    }
    str_t cookie_str = string_to_str(cookie_string);
    string_t result = string_append_format(cookies->writer, "Set-Cookie: %.*s\n", cookie_str.length, cookie_str.data);
    string_destroy(cookie_string);
    if (!result) {
        return false;
    }
    cookies->writer = result;
    return true;
}

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

void cookies_init(allocator_t* allocator, cookies_t* cookies) {
    strhashmap_init(allocator, &cookies->strhashmap);
}

void cookies_deinit(cookies_t* cookies) {
    strhashmap_deinit(&cookies->strhashmap);
}

b8 cookies_get(cookies_t* cookies, str_t key, str_t* out_value) {
    return strhashmap_get(&cookies->strhashmap, key, out_value);
}

b8 cookies_get_ci(cookies_t* cookies, str_t key, str_t* out_value) {
    return strhashmap_get_ci(&cookies->strhashmap, key, out_value);
}

b8 cookies_set(cookies_t* cookies, str_t key, str_t value) {
    return strhashmap_set(&cookies->strhashmap, key, value);
}

string_t cookies_to_string(allocator_t* allocator, cookies_t* cookies) {
    if (!cookies) {
        LOG_ERROR("cookies_to_string - called with cookies 0.");
        return 0;
    }
    return strhashmap_to_string(allocator, &cookies->strhashmap, "%.*s=%.*s");
}
