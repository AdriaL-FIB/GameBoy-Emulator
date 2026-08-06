#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define CHECK_BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#define BIT_SET(a, n, on) { if (on) a |= (1 << n); else a &= ~(1 << n);}

#define BETWEEN(n, l, h) ((n >= l) && (n <= h))

void delay(u32 ms);

#define NO_IMPL { fprintf(stderr, "NOT YET IMPLEMENTED\n"); exit(-5); }

#define IE_ADDR 0xFFFF
#define IF_ADDR 0xFF0F