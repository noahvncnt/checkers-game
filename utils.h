#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// uses A1 bit = 0 at bottom-left
// Files  AH masks and ranks masks.
extern const uint64_t FILE_A;
extern const uint64_t FILE_H;
extern const uint64_t RANK_1;
extern const uint64_t RANK_8;
extern const uint64_t DARK_SQUARES;

int square_index_from_coord(const char* coord); // e.g., "b6" -> index
void coord_from_square_index(int idx, char out[3]); // idx -> like "b6"

#endif