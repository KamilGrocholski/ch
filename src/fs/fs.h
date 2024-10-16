#pragma once

#include "core/defines.h"
#include "core/string.h"
#include "core/memory.h"

typedef struct file_handle_t {
    void* handle;
    b8 is_valid;
} file_handle_t;

typedef enum file_mode_t {
    FILE_MODE_READ = 0x1,
    FILE_MODE_WRITE = 0x2,
} file_mode_t;

b8 fs_exists(const char* path);

b8 fs_open(const char* path, file_mode_t mode, b8 binary, file_handle_t* out_handle);

void fs_close(file_handle_t* handle);

b8 fs_size(file_handle_t* handle, u64* out_size);

b8 fs_read_entire_text(const char* filepath, string_t* out_text);
