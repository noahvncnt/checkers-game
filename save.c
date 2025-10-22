#include "save.h"
#include <stdio.h>

int save_game_to_file(const GameState* g, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) return -1;
    //  text format
    fprintf(f, "red %llu\nblack %llu\nkings %llu\nturn %d\n",
            (unsigned long long)g->red,
            (unsigned long long)g->black,
            (unsigned long long)g->kings,
            (int)g->turn);
    fclose(f);
    return 0;
}

int load_game_from_file(GameState* g, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    unsigned long long red, black, kings;
    int turn;
    int n = fscanf(f, "red %llu\nblack %llu\nkings %llu\nturn %d\n",
                   &red, &black, &kings, &turn);
    fclose(f);
    if (n != 4) return -1;
    g->red = (uint64_t)red;
    g->black = (uint64_t)black;
    g->kings = (uint64_t)kings;
    g->turn = (turn==0)?RED:BLACK;
    return 0;
}