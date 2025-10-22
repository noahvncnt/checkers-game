#include <stdio.h>
#include "game.h"
#include "ui.h"

int main(void) {
    GameState g;
    game_init(&g);

    printf("Bitboard Checkers — C11\n");
    while (1) {
        if (!play_turn(&g)) break;
        int winner = check_winner(&g);
        if (winner == RED) { printf("RED wins!\n"); break; }
        if (winner == BLACK) { printf("BLACK wins!\n"); break; }
    }
    printf("Goodbye.\n");
    return 0;
}