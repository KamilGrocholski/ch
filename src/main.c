#include "core/string.h"
#include "core/logger.h"
#include "core/defines.h"
#include "core/array.h"
#include "fs/fs.h"

#include <stdio.h>
#include <assert.h>

int main() {
    string_t string = string_create(0, "siema");
    string = string_append_cstr(string, " elo!");
    string = string_append_cstr(string, " \ntak\n");
    string = string_append_cstr(string, "format\n");
    LOG_INFO(string);
    string_destroy(string);

    string_t content = fs_read_entire_text(0, "siema.txt");
    if (content) {
        LOG_INFO(content);
        string_destroy(content);
    }

    return 0;
}
