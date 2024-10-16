#include "core/memory.h"

#include "logger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char* memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "STRING     ",
    "ARENA      ",
};

typedef struct memory_stats_t {
    u64 total_allocated;
    u64 allocations_count;
    u64 deallocations_count;
    u64 tagged_allocations[MEMORY_TAG_MAX_TAGS];
    u64 tagged_allocations_count[MEMORY_TAG_MAX_TAGS];
    u64 tagged_deallocations_count[MEMORY_TAG_MAX_TAGS];
} memory_stats_t;

static memory_stats_t stats;

void memory_init(void) {
    memory_zero(&stats, sizeof(memory_stats_t));
}

void memory_shutdown(void) {
    memory_zero(&stats, sizeof(memory_stats_t));
}

void* memory_allocate(u64 size, memory_tag_t tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        LOG_WARN("memory_allocate - called with MEMORY_TAG_UNKNOWN. You should change it.");
    }
    stats.total_allocated += size;
    stats.tagged_allocations[tag] += size;
    stats.tagged_allocations_count[tag]++;
    stats.allocations_count++;
    return malloc(size);
}

void* memory_reallocate(void* block, u64 old_size, u64 new_size, memory_tag_t tag) {
    void* new_block = memory_allocate(new_size, tag);
    memory_copy(new_block, block, old_size);
    memory_free(block, old_size, tag);
    return new_block;
}

void memory_free(void* block, u64 size, memory_tag_t tag) {
    if (tag == MEMORY_TAG_UNKNOWN) {
        LOG_WARN("memory_free - called with MEMORY_TAG_UNKNOWN. You should change it.");
    }
    stats.total_allocated -= size;
    stats.tagged_allocations[tag] -= size;
    stats.tagged_deallocations_count[tag]++;
    stats.deallocations_count++;
    free(block);
}

void* memory_copy(void* dest, const void* src, u64 size) {
    return memcpy(dest, src, size);
}

void* memory_set(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void* memory_zero(void* dest, u64 size) {
    return memset(dest, 0, size);
}

char* memory_stats_to_string(void) {
    const u64 gib = 1024 * 1024 * 1024;
    const u64 mib = 1024 * 1024;
    const u64 kib = 1024;

    char buffer[8000];
    sprintf(&buffer[0], "Memory report:\n    allocs: %llu\n    deallocs: %llu\n    usage(tagged):\n", 
            stats.allocations_count, stats.deallocations_count);
    u64 offset = strlen(buffer);
    for (u32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tagged_allocations[i] >= gib) {
            unit[0] = 'G';
            amount = stats.tagged_allocations[i] / (float)gib;
        } else if (stats.tagged_allocations[i] >= mib) {
            unit[0] = 'M';
            amount = stats.tagged_allocations[i] / (float)mib;
        } else if (stats.tagged_allocations[i] >= kib) {
            unit[0] = 'K';
            amount = stats.tagged_allocations[i] / (float)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float)stats.tagged_allocations[i];
        }
        i32 length = snprintf(buffer + offset, 8000, "       %s: %.2f%s(allocs: %llu, deallocs: %llu)\n", 
                              memory_tag_strings[i], 
                              amount, 
                              unit,
                              stats.tagged_allocations_count[i],
                              stats.tagged_deallocations_count[i]);
        offset += length;
    }
    char* out_string = strdup(buffer);
    return out_string;
}

void memory_report(void) {
    char* report = memory_stats_to_string();
    printf(report);
    free(report);
}
