#include "bitops.h"

// --- 32-bit ---
uint32_t set_bit32(uint32_t value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value | (1u << pos);
}
uint32_t clear_bit32(uint32_t value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value & ~(1u << pos);
}
uint32_t toggle_bit32(uint32_t value, int pos) {
    if (pos < 0 || pos >= 32) return value;
    return value ^ (1u << pos);
}
int get_bit32(uint32_t value, int pos) {
    if (pos < 0 || pos >= 32) return 0;
    return (value >> pos) & 1u;
}
int count_bits32(uint32_t value) {
    return __builtin_popcount(value);
}
uint32_t shl32(uint32_t value, int n) {
    if (n <= 0 || n >= 32) return 0u;
    return value << n;
}
uint32_t shr32(uint32_t value, int n) {
    if (n <= 0 || n >= 32) return 0u;
    return value >> n;
}
void print_binary32(uint32_t value) {
    for (int i = 31; i >= 0; --i) {
        putchar(((value >> i) & 1u) ? '1' : '0');
        if (i % 4 == 0 && i) putchar(' ');
    }
    putchar('\n');
}

// --- 64-bit ---
uint64_t set_bit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value | (1ull << pos);
}
uint64_t clear_bit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value & ~(1ull << pos);
}
uint64_t toggle_bit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return value;
    return value ^ (1ull << pos);
}
int get_bit64(uint64_t value, int pos) {
    if (pos < 0 || pos >= 64) return 0;
    return (int)((value >> pos) & 1ull);
}
int count_bits64(uint64_t value) {
    return __builtin_popcountll(value);
}
uint64_t shl64(uint64_t value, int n) {
    if (n <= 0 || n >= 64) return 0ull;
    return value << n;
}
uint64_t shr64(uint64_t value, int n) {
    if (n <= 0 || n >= 64) return 0ull;
    return value >> n;
}
void print_binary64(uint64_t value) {
    for (int i = 63; i >= 0; --i) {
        putchar(((value >> i) & 1ull) ? '1' : '0');
        if (i % 4 == 0 && i) putchar(' ');
    }
    putchar('\n');
}

uint64_t bit_mask64(int pos) {
    if (pos < 0 || pos >= 64) return 0ull;
    return 1ull << pos;
}