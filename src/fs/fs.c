#include "fs/fs.h"

#include "core/logger.h"

#include <stdio.h>
#include <sys/stat.h>

b8 fs_exists(const char* path) {
    struct stat buffer;
    return stat(path, &buffer) == 0;
}

b8 fs_open(const char* path, file_mode_t mode, b8 binary, file_handle_t* out_handle) {
    out_handle->is_valid = false;
    out_handle->handle = 0;
    const char* mode_str;

    if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "w+b" : "w+";
    } else if ((mode & FILE_MODE_READ) != 0 && (mode & FILE_MODE_WRITE) == 0) {
        mode_str = binary ? "rb" : "r";
    } else if ((mode & FILE_MODE_READ) == 0 && (mode & FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "wb" : "w";
    } else {
        LOG_ERROR("Invalid mode passed while trying to open file: '%s'", path);
        return false;
    }

    FILE* file = fopen(path, mode_str);
    if (!file) {
        LOG_ERROR("Error opening file: '%s'", path);
        return false;
    }

    out_handle->handle = file;
    out_handle->is_valid = true;

    return true;
}

void fs_close(file_handle_t* handle) {
    if (!handle->handle) { 
        return;
    }
    fclose((FILE*)handle->handle);
    handle->handle = 0;
    handle->is_valid = false;
}

b8 fs_size(file_handle_t* handle, u64* out_size) {
    if (!handle->handle) {
        return false;
    }
    fseek((FILE*)handle->handle, 0, SEEK_END);
    *out_size = ftell((FILE*)handle->handle);
    rewind((FILE*)handle->handle);
    return true;
}

string_t fs_read_entire_text(allocator_t* allocator, const char* filepath) {
    file_handle_t f = {0};
    if (!fs_open(filepath, FILE_MODE_READ, false, &f)) {
        return 0;
    }

    u64 size = 0;
    if (!fs_size(&f, &size)) {
        fs_close(&f);
        return 0;
    }
    string_t string = string_from_parts(allocator, 0, size + 1);
    u64 bytes_read = fread(string, 1, size, (FILE*)f.handle);
    string_length_set(string, bytes_read);
    string[bytes_read] = 0;

    if (bytes_read < size) {
        string_t copy = string_duplicate(string);
        string_destroy(string);
        fs_close(&f);
        return copy;
    }

    fs_close(&f);
    return string;
}
