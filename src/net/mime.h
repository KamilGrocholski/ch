#pragma once

#include "core/defines.h"
#include "core/str.h"

typedef enum mime_t {
    MIME_APPLICATION_HTML,
    MIME_APPLICATION_JSON,
    MIME_APPLICATION_JAVASCRIPT,
    MIME_APPLICATION_STAR,
    MIME_TEXT_JAVASCRIPT,
    MIME_TEXT_PLAIN,
    MIME_TEXT_PLAIN_UTF8,
    MIME_TEXT_HTML,
    MIME_TEXT_HTML_UTF8,
    MIME_TEXT_CSS,
    MIME_TEXT_CSS_UTF8,
    MIME_TEXT_STAR,
    MIME_LENGTH,
} mime_t;

b8 mime_is_valid(mime_t mime);

b8 mime_to_str(mime_t mime, str_t* dest);

b8 mime_from_str(str_t str, mime_t* dest);
