#include "net/http.h"

#include "core/logger.h"
#include "core/assert.h"
#include "core/strhashmap.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>

b8 http_response_init(http_response_t* dest) {
    ASSERT(dest);
    strhashmap_init(0, &dest->headers);
    return true;
}

void http_response_deinit(http_response_t* response) {
    if (!response) {
        LOG_ERROR("http_response_deinit - called with response 0.");
        return false;
    }
    strhashmap_init(0, &response->headers);
    return true;
}

b8 http_response_headers_set(http_response_t* response, str_t key, str_t value) {
    return strhashmap_set(&response->headers, key, value);
}

b8 http_response_status_set(http_response_t* response, http_status_t status) {
    response->status = status;
    return true;
}

b8 http_response_send_no_content(http_response_t* response, http_status_t status) {
    return http_response_send(response, status, STR_EMPTY);
}

b8 http_response_send_text(http_response_t* response, http_status_t status, str_t text) {
    if (!http_response_status_set(response, status)) {
        return false;
    }
    if (!http_response_headers_set(response, str_from_cstr("Content-Type"), str_from_cstr("text/plain"))) {
        return false;
    }
    return http_response_send(response, status, text);
}

b8 http_response_send(http_response_t* response, http_status_t status, str_t data) {
    char data_length[64];
    sprintf(data_length, "%llu", data.length);
    if (!http_response_headers_set(response, str_from_cstr("Content-Length"), str_from_cstr(data_length))) {
        LOG_DEBUG("http_response_send - could not set Content-Length header");
        return false;
    }

    string_t string = string_create(0, "HTTP/1.1");
    if (!string) {
        LOG_DEBUG("no string");
        return false;
    }

    string = string_append_format(string, " %d", status);
    string = string_append_format(string, " %s\n", http_status_to_cstr(status));

    for (u64 i = 0; i < response->headers.capacity; i++) {
        strhashmap_entry_t entry = response->headers.table[i];
        if (entry.is_occupied) {
            str_t key = entry.key;
            str_t value = entry.value;
            string = string_append_format(string, "%.*s: %.*s\n", key.length, key.data, value.length, value.data);
        }
    }
    
    string = string_append_format(string, "\r\n%.*s", data.length, data.data);
    if (!string) {
        LOG_DEBUG("no string 2");
        return false;
    }

    LOG_DEBUG("final response:\n'%s'", string);

    i32 bytes_sent = send(response->client_fd, string, string_length(string), 0);
    if (bytes_sent == -1) {
        LOG_DEBUG("http_response_send - could not send the response");
        return false;
    }
    LOG_DEBUG("http_response_send - sent %d bytes", bytes_sent);
    return true;
}
