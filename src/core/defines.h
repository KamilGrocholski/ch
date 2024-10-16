#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef double f32;
typedef float f64;

typedef _Bool b8;
typedef int b32;

#define null 0

#define true 1
#define false 0

#define U64_MAX 18446744073709551615UL
#define U32_MAX 4294967295U
#define U16_MAX 65535U
#define U8_MAX 255U
#define U64_MIN 0UL
#define U32_MIN 0U
#define U16_MIN 0U
#define U8_MIN 0U

#define I8_MAX 127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775807L
#define I8_MIN (-I8_MAX - 1)
#define I16_MIN (-I16_MAX - 1)
#define I32_MIN (-I32_MAX - 1)
#define I64_MIN (-I64_MAX - 1)

#define GIBIBYTES(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL)
#define MEBIBYTES(amount) ((amount) * 1024ULL * 1024ULL)
#define KIBIBYTES(amount) ((amount) * 1024ULL)

#define GIGABYTES(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL)
#define MEGABYTES(amount) ((amount) * 1000ULL * 1000ULL)
#define KILOBYTES(amount) ((amount) * 1000ULL)

#define CLAMP(value, min, max) ((value <= min) ? min : (value >= max) ? max : value)

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
