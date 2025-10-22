#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void print_rank_border() {
    printf("  +---+---+---+---+---+---+---+---+\n");
}

void print_legend(void) {
    printf("Legend: r=Red man, R=Red king, b=Black man, B=Black king\n");
    printf("Enter moves as from-to (e.g., b6-a5 or c3-e5 for jumps). Use 'save <file>' or 'load <file>' or 'quit'.\n");
}

void print_board(const GameState* g) {
    print_rank_border();
    for (int r = 7; r >= 0; --r) {
        printf("%d |", r+1);
        for (int c = 0; c < 8; ++c) {
            int idx = r*8 + c;
            char ch = ' ';
            int occ_red = ( (g->red >> idx) & 1ull );
            int occ_black = ( (g->black >> idx) & 1ull );
            int king = ( (g->kings >> idx) & 1ull );
            if (occ_red) ch = king ? 'R' : 'r';
            else if (occ_black) ch = king ? 'B' : 'b';
            else {
                // show dark/light
                ch = ((r+c)&1) ? '.' : ' ';
            }
            printf(" %c |", ch);
        }
        printf("\n");
        print_rank_border();
    }
    printf("    a   b   c   d   e   f   g   h\n");
}

int parse_move(const char* s, Move* out) {
    // formats: "b6-a5", "b6c5" (optional dash), optionally with spaces
    if (!s || !out) return -1;
    char a='?',b='?', c='?', d='?';
    int i=0;
    // skip spaces
    while (s[i] && isspace((unsigned char)s[i])) i++;
    if (!isalpha((unsigned char)s[i]) || !isdigit((unsigned char)s[i+1])) return -1;
    a = s[i]; b = s[i+1];
    i += 2;
    while (s[i] && (s[i]=='-' || s[i]=='>')) i++;
    if (!isalpha((unsigned char)s[i]) || !isdigit((unsigned char)s[i+1])) return -1;
    c = s[i]; d = s[i+1];

    char froms[3] = { (char)tolower(a), b, 0 };
    char tos[3]   = { (char)tolower(c), d, 0 };
    int from = square_index_from_coord(froms);
    int to   = square_index_from_coord(tos);
    if (from < 0 || to < 0) return -1;
    out->from = from;
    out->to = to;
    out->captured_idx = -1;
    out->is_capture = 0;
    return 0;
}

static int prompt_line(char* buf, size_t n) {
    printf("> ");
    if (!fgets(buf, (int)n, stdin)) return 0;
    size_t L = strlen(buf);
    if (L && buf[L-1]=='\n') buf[L-1]='\0';
    return 1;
}

extern int save_game_to_file(const GameState* g, const char* path);
extern int load_game_from_file(GameState* g, const char* path);

int play_turn(GameState* g) {
    char line[256];
    printf("\n%s to move.\n", g->turn==RED? "Red" : "Black");
    print_board(g);
    print_legend();

    while (1) {
        if (!prompt_line(line, sizeof(line))) return 0;

        if (strncmp(line, "quit", 4)==0) return 0;

        if (strncmp(line, "save ", 5)==0) {
            const char* path = line+5;
            if (save_game_to_file(g, path)==0) printf("Saved to '%s'.\n", path);
            else printf("Failed to save '%s'.\n", path);
            continue;
        }
        if (strncmp(line, "load ", 5)==0) {
            const char* path = line+5;
            if (load_game_from_file(g, path)==0) {
                printf("Loaded '%s'.\n", path);
                print_board(g);
            } else {
                printf("Failed to load '%s'.\n", path);
            }
            continue;
        }

        Move m;
        if (parse_move(line, &m) != 0) {
            printf("Could not parse. Try like 'b6-a5'.\n");
            continue;
        }

        GameState backup = *g;
        if (!apply_move(g, m)) {
            printf("Illegal move. Try again.\n");
            *g = backup;
            continue;
        }

        // If  capture, check for multicapture possibility
        int did_capture = ( (m.to/8 - m.from/8) > 1 || (m.from/8 - m.to/8) > 1 );
        if (did_capture) {
            Move cont[16];
            int n = generate_captures_from(g, m.to, cont, 16);
            if (n > 0) {
                print_board(g);
                printf("You captured. Multi-jump available from ");
                char sq[3]; coord_from_square_index(m.to, sq);
                printf("%s. Enter next jump (from %s), or 'done' to stop (if rules allow).\n", sq, sq);
                while (n > 0) {
                    if (!prompt_line(line, sizeof(line))) return 0;
                    if (strncmp(line, "done", 4)==0) break;
                    Move m2;
                    if (parse_move(line, &m2) != 0 || m2.from != m.to) {
                        printf("Enter a jump starting from %s.\n", sq);
                        continue;
                    }
                    if (!apply_move(g, m2)) {
                        printf("Illegal continuation.\n");
                        continue;
                    }
                    // update for next potential jump
                    n = generate_captures_from(g, m2.to, cont, 16);
                    if (n>0) {
                        coord_from_square_index(m2.to, sq);
                        print_board(g);
                        printf("Another jump is available from %s. Continue or 'done'.\n", sq);
                        m = m2; // track last
                    } else break;
                }
            }
        }

        // End turn
        g->turn = (g->turn==RED) ? BLACK : RED;
        return 1;
    }
}