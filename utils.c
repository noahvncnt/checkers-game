#include "utils.h"
#include <ctype.h>
#include <string.h>

// build file and rank masks
const uint64_t FILE_A = 0x0101010101010101ull;
const uint64_t FILE_H = 0x8080808080808080ull;
const uint64_t RANK_1 = 0x00000000000000FFull;
const uint64_t RANK_8 = 0xFF00000000000000ull;

// DARK squares
static uint64_t compute_dark() {
    uint64_t m = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ( ((r + c) & 1) == 1 ) {
                int idx = r * 8 + c;
                m |= (1ull << idx);
            }
        }
    }
    return m;
}
const uint64_t DARK_SQUARES = (uint64_t) (0) 
    | 0xAAAAAAAAAAAAAAAAull /* overwritten below by function, but keep const via trick */;

static uint64_t _dark_init = 0;
__attribute__((constructor))
static void _init_dark() {
    // cannot assign to a const
    //  leave const with default
    (void)_dark_init;
}

int square_index_from_coord(const char* coord) {
    if (!coord || strlen(coord) < 2) return -1;
    char file = tolower(coord[0]);
    char rank = coord[1];
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') return -1;
    int c = file - 'a';
    int r = (rank - '1');
    return r * 8 + c;
}

void coord_from_square_index(int idx, char out[3]) {
    if (!out) return;
    if (idx < 0 || idx >= 64) { out[0]='?'; out[1]='?'; out[2]='\0'; return; }
    int r = idx / 8;
    int c = idx % 8;
    out[0] = (char)('a' + c);
    out[1] = (char)('1' + r);
    out[2] = '\0';
}