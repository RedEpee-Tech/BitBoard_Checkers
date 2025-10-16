#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "BitBoard_Checkers.h"

// Provide the requested utility functions (PascalCase) and lowercase helpers

// Set, clear, toggle, get
void SetBit(Bitboard *b, int i) { *b |= (1ULL << i); }
void ClearBit(Bitboard *b, int i) { *b &= ~(1ULL << i); }
void ToggleBit(Bitboard *b, int i) { *b ^= (1ULL << i); }
int GetBit(Bitboard b, int i) { return (int)((b >> i) & 1ULL); }

// Count bits (popcount)
int count_bits(Bitboard b) {
    int cnt = 0;
    while (b) {
        cnt += (int)(b & 1ULL);
        b >>= 1;
    }
    return cnt;
}
int CountBits(Bitboard b) { return count_bits(b); }

// Shifts
Bitboard shift_left(Bitboard b, int n) { return b << n; }
Bitboard shift_right(Bitboard b, int n) { return b >> n; }
Bitboard ShiftLeft(Bitboard b, int n) { return shift_left(b, n); }
Bitboard ShiftRight(Bitboard b, int n) { return shift_right(b, n); }

// Printing
void print_binary(Bitboard b) {
    for (int i = 63; i >= 0; --i) {
        putchar(((b >> i) & 1ULL) ? '1' : '0');
        if (i % 8 == 0) putchar(' ');
    }
    putchar('\n');
}
void PrintBinary(Bitboard b) { print_binary(b); }

void print_hex(Bitboard b) {
    printf("0x%016llX\n", (unsigned long long)b);
}
void PrintHex(Bitboard b) { print_hex(b); }

// ----------- Board Initialization -----------
void init_board(Bitboard *red, Bitboard *black, Bitboard *kings) {
    *red = 0;
    *black = 0;
    *kings = 0;

    // Black pieces on top 3 rows (rows 0–2)
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                SetBit(black, r * 8 + c);
            }
        }
    }

    // Red pieces on bottom 3 rows (rows 5–7)
    for (int r = 5; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                SetBit(red, r * 8 + c);
            }
        }
    }
}

// ----------- Square Parser -----------
int parse_square(const char *s) {
    if (strlen(s) != 2) return -1;
    char file = tolower(s[0]);  // 'a'..'h'
    char rank = s[1];           // '1'..'8'

    if (file < 'a' || file > 'h') return -1;
    if (rank < '1' || rank > '8') return -1;

    int col = file - 'a';
    int row = 8 - (rank - '0');  // rank 8 = row 0, rank 1 = row 7
    return row * 8 + col;
}

// ----------- Board Printing -----------
void print_board(Bitboard red, Bitboard black, Bitboard kings) {
    printf("\n   a  b  c  d  e  f  g  h\n");
    for (int r = 0; r < 8; r++) {
        printf("%d ", 8 - r);  // rank labels (8..1)
        for (int c = 0; c < 8; c++) {
            int i = r * 8 + c;

            // Unplayable (light) squares
            if ((r + c) % 2 == 0) {
                printf(" . ");
                continue;
            }

            // Playable (dark) squares
            if (GetBit(red, i)) {
                if (GetBit(kings, i)) printf(" R "); else printf(" r ");
            }
            else if (GetBit(black, i)) {
                if (GetBit(kings, i)) printf(" B "); else printf(" b ");
            }
            else printf(" _ ");
        }
        printf(" %d\n", 8 - r);
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
}




// ----------- Move Logic -----------
bool try_move(Bitboard *player, Bitboard *opponent, Bitboard *kings, int from, int to, bool isRedTurn) {
    if (from < 0 || from > 63 || to < 0 || to > 63) return false;
    if (!GetBit(*player, from)) return false;
    if (GetBit(*player, to) || GetBit(*opponent, to)) return false;

    bool isKing = GetBit(*kings, from);
    int rf = from / 8, cf = from % 8;
    int rt = to   / 8, ct = to   % 8;
    int rowDiff = rt - rf;
    int colDiff = ct - cf;
    int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
    int absCol = (colDiff < 0) ? -colDiff : colDiff;

    if (absRow != absCol) return false; // must be diagonal

    // Non-king pieces can only move forward
    if (!isKing) {
        int forward = isRedTurn ? -1 : 1;
        if (rowDiff != forward && absRow == 1) return false;
        if (absRow > 1 && ((isRedTurn && rowDiff > 0) || (!isRedTurn && rowDiff < 0))) return false;
    }

    // --- Simple move (one diagonal step) ---
    if (absRow == 1) {
        ClearBit(player, from);
        SetBit(player, to);

        // Move king bit if necessary
        if (GetBit(*kings, from)) {
            ClearBit(kings, from);
            SetBit(kings, to);
        }

        // Check for kinging: red reaches row 0, black reaches row 7
        if (isRedTurn && rt == 0) SetBit(kings, to);
        if (!isRedTurn && rt == 7) SetBit(kings, to);

        return true;
    }

    // --- Capture move (two diagonal steps) ---
    if (absRow == 2) {
        int midRow = (rf + rt) / 2;
        int midCol = (cf + ct) / 2;
        int midSq  = midRow * 8 + midCol;

        // Must have opponent piece between
        if (!GetBit(*opponent, midSq)) return false;

        // Perform capture
        ClearBit(opponent, midSq);
        // If captured piece was king, clear its king bit
        if (GetBit(*kings, midSq)) ClearBit(kings, midSq);

        ClearBit(player, from);
        SetBit(player, to);

        // Move king status if needed
        if (GetBit(*kings, from)) {
            ClearBit(kings, from);
            SetBit(kings, to);
        }

        // Check for kinging after landing
        if (isRedTurn && rt == 0) SetBit(kings, to);
        if (!isRedTurn && rt == 7) SetBit(kings, to);

        return true;
    }

    return false;
}

// Check if the piece at `pos` can make another capture (used for forced multiple captures)
bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn) {
    int r = pos / 8, c = pos % 8;
    int dirs[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}}; // all capture offsets
    bool isKing = GetBit(kings, pos);

    for (int i = 0; i < 4; ++i) {
        int dr = dirs[i][0];
        int dc = dirs[i][1];

        // if not a king, restrict forward direction
        if (!isKing) {
            if (isRedTurn && dr > 0) continue; // red moves up (-)
            if (!isRedTurn && dr < 0) continue; // black moves down (+)
        }

        int nr = r + dr;
        int nc = c + dc;
        int mr = r + dr/2;
        int mc = c + dc/2;
        if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
        int npos = nr*8 + nc;
        int mpos = mr*8 + mc;

        if (!GetBit(player, npos) && GetBit(opponent, mpos) && !GetBit(opponent, npos)) {
            return true;
        }
    }
    return false;

}

// ----------- Save/Load Game -----------
bool save_game(const char *filename, Bitboard red, Bitboard black, Bitboard kings, bool redTurn) {
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    // Save as unsigned long long decimal values and turn as 0/1
    if (fprintf(f, "%llu\n%llu\n%llu\n%d\n", (unsigned long long)red, (unsigned long long)black, (unsigned long long)kings, redTurn ? 1 : 0) < 0) {
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

bool load_game(const char *filename, Bitboard *red, Bitboard *black, Bitboard *kings, bool *redTurn) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;
    unsigned long long r, b, k;
    int turn;
    if (fscanf(f, "%llu %llu %llu %d", &r, &b, &k, &turn) < 4) {
        fclose(f);
        return false;
    }
    *red = (Bitboard)r;
    *black = (Bitboard)b;
    *kings = (Bitboard)k;
    *redTurn = (turn != 0);
    fclose(f);
    return true;
}
// ----------- Main Game Loop -----------
// int main(void) {
//     setvbuf(stdout, NULL, _IONBF, 0); // flush output immediately (for VS Code)

//     Bitboard red, black;
//     init_board(&red, &black);
//     bool redTurn = true;

//     char from[8], to[8];
//     printf("=== Bitboard Checkers ===\n");
//     printf("Enter moves like: b6 a5 or capture like: b3 d5\n");
//     printf("Type 'quit' to exit.\n");

//     print_board(red, black);
//     while (1) {
//         printf("%s's move: ", redTurn ? "Red" : "Black");
//         if (scanf("%7s", from) != 1) break;
//         if (strcasecmp(from, "quit") == 0) break;
//         if (scanf("%7s", to) != 1) break;
//         int fromSq = parse_square(from);
//         int toSq   = parse_square(to);
//         if (fromSq < 0 || toSq < 0) {
//             printf("Invalid coordinates. Try again.\n");
//             continue;
// int main(void) {
//     setvbuf(stdout, NULL, _IONBF, 0); // flush output immediately (for VS Code)

//     Bitboard red, black, kings;
//     init_board(&red, &black, &kings);
//     bool redTurn = true;

//     char from[8], to[8];
//     printf("=== Bitboard Checkers ===\n");
//     printf("Enter moves like: b6 a5 or capture like: b3 d5\n");
//     printf("Type 'quit' to exit.\n");

//     print_board(red, black, kings);

//     while (1) {
//         printf("%s's move: ", redTurn ? "Red" : "Black");

//         if (scanf("%7s", from) != 1) break;
//         if (strcasecmp(from, "quit") == 0) break;
//         if (scanf("%7s", to) != 1) break;

//         int fromSq = parse_square(from);
//         int toSq   = parse_square(to);

//         if (fromSq < 0 || toSq < 0) {
//             printf("Invalid coordinates. Try again.\n");
//             continue;
//         }

//         Bitboard *current = redTurn ? &red : &black;
//         Bitboard *enemy   = redTurn ? &black : &red;
//         Bitboard *kings   = redTurn ? &kings : &red;


//         if (try_move(current, enemy, kings, fromSq, toSq, redTurn)) {
//             print_board(red, black, kings);
//             redTurn = !redTurn;
//         } else {
//             printf("Illegal move. Try again.\n");
//         }
//     }

//     printf("Game ended.\n");
//     return 0;
// }

int main() {
    Bitboard red, black, kings;
    bool redTurn = true;
    char cmd[64];
    char extra[64];
    char filename[128];

    printf("Welcome to Bitboard Checkers!\n");
    printf("Commands:\n");
    printf("  move <from> <to>         - e.g. move b6 a5\n");
    printf("  save <filename>          - e.g. save mygame.txt\n");
    printf("  load <filename>          - e.g. load mygame.txt\n");
    printf("  exit\n");

    init_board(&red, &black, &kings);

    while (1) {
        print_board(red, black, kings);
        printf("%s's turn > ", redTurn ? "Red" : "Black");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;

        // Strip trailing newline
        size_t len = strlen(cmd);
        if (len && cmd[len-1] == '\n') cmd[len-1] = 0;

        if (strncmp(cmd, "exit", 4) == 0) break;

        if (sscanf(cmd, "save %127s", filename) == 1) {
            if (save_game(filename, red, black, kings, redTurn))
                printf("Game saved to %s\n", filename);
            else
                printf("Failed to save game.\n");
            continue;
        }

        if (sscanf(cmd, "load %127s", filename) == 1) {
            if (load_game(filename, &red, &black, &kings, &redTurn))
                printf("Game loaded from %s\n", filename);
            else
                printf("Failed to load game.\n");
            continue;
        }

        // Move command
        char from_square[8] = {0}, to_square[8] = {0};
        if (sscanf(cmd, "move %7s %7s %63s", from_square, to_square, extra) >= 2) {
            int from = parse_square(from_square);
            int to   = parse_square(to_square);
            Bitboard *player = redTurn ? &red : &black;
            Bitboard *opponent = redTurn ? &black : &red;

            if (try_move(player, opponent, &kings, from, to, redTurn)) {
                // Check forced multiple captures
                bool multi_capture = false;
                int cur_pos = to;
                while (can_capture_more(*player, *opponent, kings, cur_pos, redTurn)) {
                    print_board(red, black, kings);
                    printf("%s must continue capture from %c%d\n", redTurn ? "Red" : "Black",
                        'a' + (cur_pos % 8), 8 - (cur_pos / 8));
                    printf("Next move > ");
                    if (!fgets(cmd, sizeof(cmd), stdin)) break;
                    if (sscanf(cmd, "%7s", to_square) != 1) break;
                    int next_to = parse_square(to_square);
                    if (!try_move(player, opponent, &kings, cur_pos, next_to, redTurn)) {
                        printf("Invalid multi-capture move. Turn forfeit.\n");
                        break;
                    }
                    cur_pos = next_to;
                    multi_capture = true;
                }
                redTurn = !redTurn;
            } else {
                printf("Invalid move!\n");
            }
            continue;
        }

        printf("Unknown or malformed command.\n");
    }

    printf("Thanks for playing!\n");
    return 0;
}
