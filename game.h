#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { RED = 0, BLACK = 1 } Player;

typedef struct {
    uint64_t red;       // regular red men
    uint64_t black;     // regular black men
    uint64_t kings;     // squares that are kings (either color)
    Player turn;        // whose turn it is
} GameState;

// init
void game_init(GameState* g);

// Move represent
typedef struct {
    int from;
    int to;
    bool is_capture;
    int captured_idx; // -1 if none
} Move;

// Rules / generation
uint64_t occupied(const GameState* g);
uint64_t red_pieces(const GameState* g);
uint64_t black_pieces(const GameState* g);
uint64_t empty_squares(const GameState* g);

bool is_king(const GameState* g, int idx);
bool is_red(const GameState* g, int idx);
bool is_black(const GameState* g, int idx);

// Generate simple moves and captures for current player
int generate_legal_moves(const GameState* g, Move* out, int max_out);

// Apply a move
bool apply_move(GameState* g, Move m);

// Check win condition
int check_winner(const GameState* g);

// Util multi-jump detection
int generate_captures_from(const GameState* g, int from_idx, Move* out, int max_out);

#endif