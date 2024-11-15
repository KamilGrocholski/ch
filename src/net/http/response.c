#include "net/http.h"

#include "core/logger.h"
#include "core/assert.h"
#include "core/strhashmap.h"
#include "fs/fs.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

b8 http_response_init(http_response_t* dest) {
    ASSERT(dest);
    strhashmap_init(0, &dest->headers);
    return true;
}

void http_response_deinit(http_response_t* response) {
    if (!response) {
        LOG_ERROR("http_response_deinit - called with response 0.");
        return;
    }
    strhashmap_deinit(&response->headers);
}

b8 http_response_headers_set(http_response_t* response, str_t key, str_t value) {
    return strhashmap_set(&response->headers, key, value);
}

b8 http_response_status_set(http_response_t* response, http_status_t status) {
    response->status = status;
    return true;
}

http_result_t http_response_send_no_content(http_response_t* response, http_status_t status) {
    return http_response_send(response, status, STR_EMPTY);
}

http_result_t http_response_send_text(http_response_t* response, http_status_t status, str_t text) {
    if (!http_response_headers_set(response, str_from_cstr("Content-Type"), str_from_cstr("text/plain"))) {
        LOG_DEBUG("http_response_send_text - could not set header");
        return HTTP_RESULT_ERR;
    }
    return http_response_send(response, status, text);
}

http_result_t http_response_send_file(http_response_t* response, http_status_t status, const char* path) {
    if (!fs_exists(path)) {
        LOG_DEBUG("http_response_send_file - file does not exist: '%s'", path);
        return HTTP_RESULT_ERR;
    }
    string_t content = fs_read_entire_text(0, path);
    if (!content) {
        LOG_DEBUG("http_response_send_file - content is 0");
        return HTTP_RESULT_ERR;
    }
    
    http_result_t result = http_response_send(response, status, string_to_str(content));
    string_destroy(content);

    return result;
}

http_result_t http_response_send(http_response_t* response, http_status_t status, str_t content) {
    char content_length_literal[64];
    snprintf(content_length_literal, sizeof(content_length_literal), "%llu", content.length + 1);
    if (!http_response_headers_set(response, str_from_cstr("Content-Length"), str_from_cstr(content_length_literal))) {
        LOG_DEBUG("http_response_send - could not set Content-Length header");
        return HTTP_RESULT_ERR;
    }

    string_t string = string_create(0, "HTTP/1.1");
    if (!string) {
        LOG_DEBUG("http_response_send - could not allocate initial string");
        return HTTP_RESULT_ERR;
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

    string = string_append_format(string, "\n\n%.*s", content.length, content.data);
    if (!string) {
        LOG_DEBUG("http_response_send - no string 2");
        return HTTP_RESULT_ERR;
    }

    LOG_DEBUG("http_response_send - final response:\n'%s'", string);
    i32 bytes_sent = 0;
    i32 bytes_total = (i32)string_length(string);
    LOG_DEBUG("http_response_send - to send %d bytes", bytes_total);
    i32 n = 0;
    while(bytes_sent < bytes_total) {
        n++;
        LOG_DEBUG("http_response_send - stream iteration n: %d", n);
        i32 segment_bytes_sent = send(response->client_fd, string + bytes_sent, bytes_total - bytes_sent, 0);
        if (segment_bytes_sent < 0) {
            LOG_ERROR("http_response_send - could not send response bytes_sent/bytes_total = %d/%d errno: %d, error: %s", 
                        bytes_sent, bytes_total, errno, strerror(errno));
            string_destroy(string);
            return HTTP_RESULT_ERR;
        }
        bytes_sent += segment_bytes_sent;
    }
    LOG_DEBUG("http_response_send - sent %d bytes", bytes_sent);
    string_destroy(string);
    return HTTP_RESULT_SEND;
}
