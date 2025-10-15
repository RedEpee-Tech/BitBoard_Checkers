#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// ----------- Bitboard Macros -----------
typedef unsigned long long Bitboard;
#define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
#define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
#define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// ----------- Board Initialization -----------
void init_board(Bitboard *red, Bitboard *black) {
    *red = 0;
    *black = 0;

    // Black pieces on top 3 rows (rows 0–2)
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                SET_BIT(*black, r * 8 + c);
            }
        }
    }

    // Red pieces on bottom 3 rows (rows 5–7)
    for (int r = 5; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1) {
                SET_BIT(*red, r * 8 + c);
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
void print_board(Bitboard red, Bitboard black) {
    printf("\n   a  b  c  d  e  f  g  h\n");
    for (int r = 0; r < 8; r++) {
        printf("%d ", 8 - r);  // rank labels (8..1)
        for (int c = 0; c < 8; c++) {
            int i = r * 8 + c;

            // Unplayable (light) squares
            if ((r + c) % 2 == 0) {
                printf(" X ");
                continue;
            }

            // Playable (dark) squares
            if (CHECK_BIT(red, i)) printf(" r ");
            else if (CHECK_BIT(black, i)) printf(" b ");
            else printf(" _ ");
        }
        printf(" %d\n", 8 - r);
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
}

// ----------- Move Logic -----------
bool try_move(Bitboard *player, Bitboard *opponent, int from, int to, bool isRedTurn) {
    if (from < 0 || from > 63 || to < 0 || to > 63) return false;
    if (!CHECK_BIT(*player, from)) return false;
    if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

    int rf = from / 8, cf = from % 8;
    int rt = to   / 8, ct = to   % 8;
    int rowDiff = rt - rf;
    int colDiff = ct - cf;
    int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
    int absCol = (colDiff < 0) ? -colDiff : colDiff;

    if (absRow != absCol) return false; // must be diagonal

    // Red moves upward (decreasing row index), black downward
    int forward = isRedTurn ? -1 : 1;

    // --- Simple move (one diagonal step forward only) ---
    if (absRow == 1) {
        if (rowDiff != forward) return false; // must move forward
        CLEAR_BIT(*player, from);
        SET_BIT(*player, to);
        return true;
    }

    // --- Capture move (two diagonal steps) ---
    if (absRow == 2) {
        int midRow = (rf + rt) / 2;
        int midCol = (cf + ct) / 2;
        int midSq  = midRow * 8 + midCol;

        // Must have opponent piece between
        if (!CHECK_BIT(*opponent, midSq)) return false;

        // Perform capture
        CLEAR_BIT(*opponent, midSq);
        CLEAR_BIT(*player, from);
        SET_BIT(*player, to);
        return true;
    }

    return false;
}

// ----------- Main Game Loop -----------
int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0); // flush output immediately (for VS Code)

    Bitboard red, black;
    init_board(&red, &black);
    bool redTurn = true;

    char from[8], to[8];
    printf("=== Bitboard Checkers ===\n");
    printf("Enter moves like: b6 a5 or capture like: b3 d5\n");
    printf("Type 'quit' to exit.\n");

    print_board(red, black);

    while (1) {
        printf("%s's move: ", redTurn ? "Red" : "Black");

        if (scanf("%7s", from) != 1) break;
        if (strcasecmp(from, "quit") == 0) break;
        if (scanf("%7s", to) != 1) break;

        int fromSq = parse_square(from);
        int toSq   = parse_square(to);

        if (fromSq < 0 || toSq < 0) {
            printf("Invalid coordinates. Try again.\n");
            continue;
        }

        Bitboard *current = redTurn ? &red : &black;
        Bitboard *enemy   = redTurn ? &black : &red;

        if (try_move(current, enemy, fromSq, toSq, redTurn)) {
            print_board(red, black);
            redTurn = !redTurn;
        } else {
            printf("Illegal move. Try again.\n");
        }
    }

    printf("Game ended.\n");
    return 0;
}
