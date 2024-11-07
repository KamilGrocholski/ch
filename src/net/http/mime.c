#include "net/http.h"

static const char* _mime_cstrs[MIME_LENGTH] = {
    [MIME_APPLICATION_HTML] = "application/html",
    [MIME_APPLICATION_JSON] = "application/json",
    [MIME_APPLICATION_JAVASCRIPT] = "application/javascript",
    [MIME_APPLICATION_STAR] = "application/*",
    [MIME_TEXT_JAVASCRIPT] = "text/javascript",
    [MIME_TEXT_PLAIN] = "text/plain",
    [MIME_TEXT_PLAIN_UTF8] = "text/plain; charset=utf-8",
    [MIME_TEXT_HTML] = "text/html",
    [MIME_TEXT_HTML_UTF8] = "text/html; charset=utf-8",
    [MIME_TEXT_CSS] = "text/css",
    [MIME_TEXT_CSS_UTF8] = "text/css; charset=utf-8",
    [MIME_TEXT_STAR] = "text/*"
};

b8 mime_is_valid(mime_t mime) {
    return mime >= 0 && mime < MIME_LENGTH;
}

b8 mime_to_str(mime_t mime, str_t* dest) {
    if (!mime_is_valid(mime)) {
        return false;
    }
    *dest = str_from_cstr(_mime_cstrs[mime]);
    return true;
}

b8 mime_from_str(str_t str, mime_t* dest) {
    for (u64 i = 0; i < MIME_LENGTH; i++) {
        // TODO remove str_from_cstr usage from mime
        str_t mime_str = str_from_cstr(_mime_cstrs[i]);
        if (str_compare(str, mime_str)) {
            *dest = i;
            return true;
        }
    }
    return false;
}
