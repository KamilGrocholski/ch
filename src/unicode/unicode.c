#include "unicode/unicode.h"

i32 utf8_encode(u32 codepoint, char* output, u32 output_length) {
    if (output_length < 1) return 0;

    if (codepoint <= 0x7F) {
        if (output_length < 1) {
            return 0;
        }
        output[0] = (char)codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        if (output_length < 2) {
            return 0;
        }
        output[0] = (char)((codepoint >> 6) | 0xC0);
        output[1] = (char)((codepoint & 0x3F) | 0x80);
        return 2;
    } else if (codepoint <= 0xFFFF) {
        if (output_length < 3) {
            return 0;
        }
        output[0] = (char)((codepoint >> 12) | 0xE0);
        output[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        output[2] = (char)((codepoint & 0x3F) | 0x80);
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        if (output_length < 4) {
            return 0;
        }
        output[0] = (char)((codepoint >> 18) | 0xF0);
        output[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        output[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        output[3] = (char)((codepoint & 0x3F) | 0x80);
        return 4;
    }

    return 0;
}

i32 utf8_decode(const char* input, u32 input_length, u32* codepoint) {
    if (input_length < 1) {
        return 0;
    }
    u8 b0 = input[0];
    if (b0 <= 0x7f) {
        *codepoint = b0;
        return 1;
    } else if ((b0 & 0xe0) == 0xc0 && input_length >= 2) {
        *codepoint = (b0 & 0x1f) << 6 | (input[1] & 0x3f);
        return 2;
    } else if ((b0 & 0xf0) == 0xe0 && input_length >= 3) {
        *codepoint = (b0 & 0x0f) << 12 | (input[1] & 0x3f) << 6 | (input[2] & 0x3f);
        return 3;
    } else if ((b0 & 0xf8) == 0xf0 && input_length >= 4) {
        *codepoint = (b0 & 0x07) << 18 | (input[1] & 0x3f) << 12 | (input[2] & 0x3f) << 6 | (input[3] & 0x3f);
        return 4;
    }
    return 0;
}

b8 is_unicode_digit(u32 codepoint) {
    return (codepoint >= 0x30 && codepoint <= 0x39);
}

b8 is_unicode_whitespace(u32 codepoint) {
    switch(codepoint) {
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x20:
            return true;

        case 0xa0:  
        case 0x1680:
        case 0x2000:
        case 0x2001:
        case 0x2002:
        case 0x2003:
        case 0x2004:
        case 0x2005:
        case 0x2006:
        case 0x2007:
        case 0x2008:
        case 0x2009:
        case 0x200a:
        case 0x2028:
        case 0x2029:
        case 0x202f:
        case 0x205f:
        case 0x3000:
            return true;

        default:
            return false;
    }
}
