#ifndef SAVE_H
#define SAVE_H

#include "game.h"

int save_game_to_file(const GameState* g, const char* path);
int load_game_from_file(GameState* g, const char* path);

#endif