#include "game.h"
#include "utils.h"
#include "bitops.h"
#include <string.h>

// Direction deltas with index mapping (A1=0 bottom-left):
// NE: +9, NW: +7, SE: -7, SW: -9
static const int NE = 9, NW = 7, SE = -7, SW = -9;

static inline int on_board(int idx) { return idx >= 0 && idx < 64; }

uint64_t occupied(const GameState* g) { return g->red | g->black; }
uint64_t empty_squares(const GameState* g) { return ~occupied(g); }
uint64_t red_pieces(const GameState* g) { return g->red; }
uint64_t black_pieces(const GameState* g) { return g->black; }

bool is_king(const GameState* g, int idx) {
    if (!on_board(idx)) return false;
    return get_bit64(g->kings, idx) != 0;
}
bool is_red(const GameState* g, int idx) {
    if (!on_board(idx)) return false;
    return get_bit64(g->red, idx) != 0;
}
bool is_black(const GameState* g, int idx) {
    if (!on_board(idx)) return false;
    return get_bit64(g->black, idx) != 0;
}

// file boundary helper
static inline int same_diagonal_step_ok(int from, int to) {
    int df = (to % 8) - (from % 8);
    int dr = (to / 8) - (from / 8);
    if (df == 1 && dr == 1) return 1; // NE
    if (df == -1 && dr == 1) return 1; // NW
    if (df == 1 && dr == -1) return 1; // SE
    if (df == -1 && dr == -1) return 1; // SW
    return 0;
}
static inline int same_jump_ok(int from, int to) {
    int df = (to % 8) - (from % 8);
    int dr = (to / 8) - (from / 8);
    if (df == 2 && dr == 2) return 1;   // NE jump
    if (df == -2 && dr == 2) return 1;  // NW jump
    if (df == 2 && dr == -2) return 1;  // SE jump
    if (df == -2 && dr == -2) return 1; // SW jump
    return 0;
}

void game_init(GameState* g) {
    memset(g, 0, sizeof(*g));
    //  12 red men on ranks 1-3 on dark squares, and 12 black men on ranks 6-8.
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            int idx = r*8 + c;
            if ( ((r + c) & 1) == 1 ) { // dark squares only
                if (r <= 2) {
                    g->red |= (1ull << idx);
                } else if (r >= 5) {
                    g->black |= (1ull << idx);
                }
            }
        }
    }
    g->kings = 0ull;
    g->turn = RED;
}

static inline void promote_if_reached_backrank(GameState* g, int idx_to) {
    int r = idx_to / 8;
    if (is_red(g, idx_to) && r == 7) {
        g->kings |= (1ull << idx_to);
    }
    if (is_black(g, idx_to) && r == 0) {
        g->kings |= (1ull << idx_to);
    }
}

static inline int dir_ok_for_piece(const GameState* g, int from, int step) {
    int king = is_king(g, from);
    if (king) return 1;
    if (is_red(g, from)) {
        // Red moves up: NE/NW positive dr
        return (step == NE || step == NW || step == (2*NE) || step == (2*NW));
    } else {
        // Black moves down: SE/SW negative dr
        return (step == SE || step == SW || step == (2*SE) || step == (2*SW));
    }
}

static inline int between_index(int from, int to) {
    // Assumes jump of two diagonals.
    int mid_r = ((from/8) + (to/8)) / 2;
    int mid_c = ((from%8) + (to%8)) / 2;
    return mid_r * 8 + mid_c;
}

static void add_move(Move* out, int* n, int from, int to, int captured) {
    out[*n].from = from;
    out[*n].to = to;
    out[*n].is_capture = (captured >= 0);
    out[*n].captured_idx = captured;
    (*n)++;
}

static int generate_simple_moves_from(const GameState* g, int from, Move* out, int max_out) {
    if (max_out <= 0) return 0;
    int count = 0;
    int dirs[4] = {NE,NW,SE,SW};
    for (int i=0;i<4;i++) {
        int step = dirs[i];
        if (!dir_ok_for_piece(g, from, step)) continue;
        int to = from + step;
        if (!on_board(to)) continue;
        if (!same_diagonal_step_ok(from, to)) continue;
        if ( ((occupied(g) >> to) & 1ull) == 0ull ) {
            add_move(out,&count,from,to,-1);
            if (count>=max_out) return count;
        }
    }
    return count;
}

int generate_captures_from(const GameState* g, int from, Move* out, int max_out) {
    if (max_out <= 0) return 0;
    int count = 0;
    int dirs[4] = {NE,NW,SE,SW};
    uint64_t me = is_red(g, from) ? (g->red) : (g->black);
    uint64_t opp = is_red(g, from) ? (g->black) : (g->red);

    for (int i=0;i<4;i++) {
        int d1 = (dirs[i]);
        int d2 = d1*2;
        if (!dir_ok_for_piece(g, from, d2)) continue;
        int over = from + d1;
        int to   = from + d2;
        if (!on_board(over) || !on_board(to)) continue;
        if (!same_jump_ok(from, to)) continue;
        if ( ((opp >> over) & 1ull) && (((occupied(g) >> to) & 1ull) == 0ull) ) {
            add_move(out,&count,from,to,over);
            if (count>=max_out) return count;
        }
    }
    return count;
}

static int any_capture_available(const GameState* g, Player p) {
    Move tmp[48];
    int n = 0;
    uint64_t pieces = (p==RED) ? g->red : g->black;
    for (int i=0;i<64;i++) if ( (pieces >> i) & 1ull ) {
        n += generate_captures_from(g, i, tmp+n, 48-n);
        if (n>0) return 1;
    }
    return 0;
}

int generate_legal_moves(const GameState* g, Move* out, int max_out) {
    int count = 0;
    int force_capture = any_capture_available(g, g->turn);
    uint64_t pieces = (g->turn==RED) ? g->red : g->black;

    for (int i=0;i<64;i++) if ( ((pieces >> i) & 1ull) ) {
        if (force_capture) {
            count += generate_captures_from(g, i, out+count, max_out-count);
        } else {
            count += generate_simple_moves_from(g, i, out+count, max_out-count);
        }
        if (count>=max_out) break;
    }
    return count;
}

bool apply_move(GameState* g, Move m) {
    if (!on_board(m.from) || !on_board(m.to)) return false;
    if ( ((occupied(g) >> m.from) & 1ull) == 0ull ) return false;
    if ( ((occupied(g) >> m.to)   & 1ull) != 0ull ) return false;
    // Must be moving piece
    int moving_red = is_red(g, m.from);
    if ( (g->turn==RED && !moving_red) || (g->turn==BLACK && moving_red) ) return false;

    int df = (m.to % 8) - (m.from % 8);
    int dr = (m.to / 8) - (m.from / 8);
    int abs_df = df<0?-df:df;
    int abs_dr = dr<0?-dr:dr;

    int king = is_king(g, m.from);
    // direction rule
    if (!king) {
        if (moving_red && dr <= 0) return false;
        if (!moving_red && dr >= 0) return false;
    }
    // Step must be diagonal
    if (!(abs_df == abs_dr && (abs_df==1 || abs_df==2))) return false;

    // Capture handling
    int is_cap = (abs_df==2);
    if (is_cap) {
        int mid = between_index(m.from, m.to);
        if (mid < 0 || mid >= 64) return false;
        // mid must be opponent
        if (moving_red) {
            if (!is_black(g, mid)) return false;
        } else {
            if (!is_red(g, mid)) return false;
        }
        // remove opponent at mid
        if (moving_red) g->black = clear_bit64(g->black, mid);
        else            g->red   = clear_bit64(g->red, mid);
        // also if that opponent was a king, clear king bit at mid
        if (get_bit64(g->kings, mid)) g->kings = clear_bit64(g->kings, mid);
    } else {
        // If any capture available , simple move is illegal
        if (any_capture_available(g, g->turn)) return false;
    }

    // Move the piece
    if (moving_red) {
        g->red = clear_bit64(g->red, m.from);
        g->red = set_bit64(g->red, m.to);
    } else {
        g->black = clear_bit64(g->black, m.from);
        g->black = set_bit64(g->black, m.to);
    }
    // Move king bit if it was king
    if (king) {
        g->kings = clear_bit64(g->kings, m.from);
        g->kings = set_bit64(g->kings, m.to);
    }

    // Promote if reaches last rank
    promote_if_reached_backrank(g, m.to);

    // If capture, check for multi-capture possibility; UI will handle offering continuation.
    return true;
}

int check_winner(const GameState* g) {
    if (g->red == 0ull) return BLACK;
    if (g->black == 0ull) return RED;

    Move buf[64];
    GameState tmp = *g;
    int n = generate_legal_moves(&tmp, buf, 64);
    if (n == 0) {
        // current player has no legal moves; the other wins
        return (g->turn == RED) ? BLACK : RED;
    }
    return -1;
}