#pragma once

#include "core/defines.h"

#define UNICODE_MAX_CODEPOINT (0x10ffff)
#define UTF8_1_BYTE_MASK (0x80) // 0xxxxxxx
#define UTF8_2_BYTE_MASK (0xc0) // 110xxxxx
#define UTF8_3_BYTE_MASK (0xe0) // 1110xxxx
#define UTF8_4_BYTE_MASK (0xf0) // 11110xxx

i32 utf8_encode(u32 codepoint, char* output, u32 output_length);

i32 utf8_decode(const char* input, u32 input_length, u32* codepoint);

b8 is_unicode_digit(u32 codepoint);

b8 is_unicode_whitespace(u32 codepoint);
