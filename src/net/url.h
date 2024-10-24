#pragma once

#include "core/defines.h"
#include "core/str.h"
#include "core/string.h"
#include "core/strhashmap.h"
#include "core/memory.h"

typedef struct url_t {
    str_t scheme;
    str_t port;
    str_t host;
    str_t path;
    str_t raw_query;
    str_t raw_fragment;
} url_t;

typedef struct url_query_t {
    strhashmap_t values;
} url_query_t;

b8 url_parse(str_t raw_request, url_t* dest);

void url_query_init(allocator_t* allocator, url_query_t* dest);

void url_query_deinit(url_query_t* query);

b8 url_query_parse(str_t raw_query, url_query_t* dest);

b8 url_query_set(url_query_t* query, str_t key, str_t value);

b8 url_query_get(url_query_t* query, str_t key, str_t* dest);

b8 url_query_remove(url_query_t* query, str_t key);

string_t url_query_to_string(allocator_t* allocator, url_query_t* query);
