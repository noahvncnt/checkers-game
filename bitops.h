#ifndef BITOPS_H
#define BITOPS_H

#include <stdio.h>
#include <stdint.h>

//bit manipulation helpers (32-bit)
uint32_t set_bit32(uint32_t value, int pos);
uint32_t clear_bit32(uint32_t value, int pos);
uint32_t toggle_bit32(uint32_t value, int pos);
int      get_bit32(uint32_t value, int pos);
int      count_bits32(uint32_t value);
uint32_t shl32(uint32_t value, int n);
uint32_t shr32(uint32_t value, int n);
void     print_binary32(uint32_t value);

//bit manipulation helpers (64-bit)
uint64_t set_bit64(uint64_t value, int pos);
uint64_t clear_bit64(uint64_t value, int pos);
uint64_t toggle_bit64(uint64_t value, int pos);
int      get_bit64(uint64_t value, int pos);
int      count_bits64(uint64_t value);
uint64_t shl64(uint64_t value, int n);
uint64_t shr64(uint64_t value, int n);
void     print_binary64(uint64_t value);


uint64_t bit_mask64(int pos);

#endif // BITOPS_H