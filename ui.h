#ifndef UI_H
#define UI_H

#include "game.h"

void print_board(const GameState* g);
void print_legend(void);

//  turn loop
int play_turn(GameState* g);

// Parse a move like "b6-a5" or "b6-c7"returns 0 on success
int parse_move(const char* s, Move* out);

#endif