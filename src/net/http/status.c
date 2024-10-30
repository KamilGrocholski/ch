#include "net/http.h"

#include "core/str.h"

const char* http_status_to_cstr(http_status_t status) {
    switch (status) {
        case HTTP_STATUS_OK:                    return "OK";
        case HTTP_STATUS_CREATED:               return "Created";
        case HTTP_STATUS_ACCEPTED:              return "Accepted";
        case HTTP_STATUS_NON_AUTHORITATIVE_INFO: return "Non-Authoritative Information";
        case HTTP_STATUS_NO_CONTENT:            return "No Content";
        case HTTP_STATUS_FOUND:                 return "Found";
        case HTTP_STATUS_SEE_OTHER:             return "See Other";
        case HTTP_STATUS_NOT_MODIFIED:          return "Not Modified";
        case HTTP_STATUS_BAD_REQUEST:           return "Bad Request";
        case HTTP_STATUS_UNAUTHORIZED:          return "Unauthorized";
        case HTTP_STATUS_FORBIDDEN:             return "Forbidden";
        case HTTP_STATUS_NOT_FOUND:             return "Not Found";
        case HTTP_STATUS_METHOD_NOT_ALLOWED:    return "Method Not Allowed";
        case HTTP_STATUS_NOT_ACCEPTABLE:        return "Not Acceptable";
        case HTTP_STATUS_REQUEST_TIMEOUT:       return "Request Timeout";
        case HTTP_STATUS_CONFLICT:              return "Conflict";
        case HTTP_STATUS_TOO_MANY_REQUESTS:     return "Too Many Requests";
        case HTTP_STATUS_INTERNAL_SERVER_ERROR: return "Internal Server Error";
        default:                                return "Unknown Status";
    }
}

str_t http_status_to_str_t(http_status_t status) {
    const char *cstr = http_status_to_cstr(status);
    return str_from_cstr(cstr);
}
