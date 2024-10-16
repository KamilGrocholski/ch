#include "core/defines.h"
#include "core/logger.h"
#include "core/assert.h"

#include "core/arena.h"
#include "core/memory.h"
#include "core/array.h"
#include "core/string.h"
#include "core/str.h"

#include "fs/fs.h"

int main() {
    string_t content = fs_read_entire_text(0, "./siema.txt");
    memory_report();
    LOG_INFO("len: %llu", content.length);

    if (!content.data) {
        LOG_ERROR("no content");
    } else {
        LOG_INFO("ELSE %.*s", content.length, content.data);
        string_destroy(&content);
    }

    memory_report();

    return 0;
}
