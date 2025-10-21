
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// ========================
// BIT MANIPULATION HELPERS
// ========================

unsigned int SetBit(unsigned int value, int position) {
    return value | (1U << position);
}

unsigned int ClearBit(unsigned int value, int position) {
    return value & ~(1U << position);
}

unsigned int ToggleBit(unsigned int value, int position) {
    return value ^ (1U << position);
}

int GetBit(unsigned int value, int position) {
    return (value >> position) & 1U;
}

int CountBits(unsigned int value) {
    int count = 0;
    while (value) {
        count += value & 1U;
        value >>= 1;
    }
    return count;
}

unsigned int ShiftLeft(unsigned int value, int positions) {
    return value << positions;
}

unsigned int ShiftRight(unsigned int value, int positions) {
    return value >> positions;
}

void PrintBinary(unsigned int value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", GetBit(value, i));
        if (i % 8 == 0 && i != 0) printf(" ");
    }
    printf("\n");
}

void PrintHex(unsigned int value) {
    printf("0x%X\n", value);
}

// ========================
// CHECKERS BITBOARD LOGIC
// ========================

#define BOARD_SIZE 8

typedef struct {
    unsigned int redPieces;
    unsigned int blackPieces;
    unsigned int kings;
} BitBoard;

int squareToIndex(char file, char rank) {
    if (!isalpha((unsigned char)file) || !isdigit((unsigned char)rank)) return -1;
    int col = tolower((unsigned char)file) - 'a';
    int row = rank - '1';
    if (col < 0 || col > 7 || row < 0 || row > 7) return -1;
    if ((row + col) % 2 == 0) return -1;
    return row * 4 + (col / 2);
}

int squareToIndexStr(const char *s) {
    if (!s || strlen(s) < 2) return -1;
    return squareToIndex(s[0], s[1]);
}
//testing if win function works
// void initBoard(BitBoard *board) {
//     // Only one red piece, no black pieces — instant Red win
//     board->redPieces = 0b00000000000000000000000000000001; // one red piece at b1
//     board->blackPieces = 0;
//     board->kings = 0;
// }

void initBoard(BitBoard *board) {
    board->redPieces   = 0x00000FFFu; // bottom three rows
    board->blackPieces = 0xFFF00000u; // top three rows
    board->kings       = 0u;
}

void printBoard(BitBoard *board) {
    printf("\n   a b c d e f g h\n");
    for (int row = BOARD_SIZE - 1; row >= 0; row--) {
        printf("%d  ", row + 1);
        for (int col = 0; col < BOARD_SIZE; col++) {
            if ((row + col) % 2 == 0) {
                printf("* ");
                continue;
            }
            int index = row * 4 + (col / 2);
            char ch = '_';
            if (GetBit(board->redPieces, index))
                ch = GetBit(board->kings, index) ? 'R' : 'r';
            else if (GetBit(board->blackPieces, index))
                ch = GetBit(board->kings, index) ? 'B' : 'b';
            printf("%c ", ch);
        }
        printf(" %d\n", row + 1);
    }
    printf("   a b c d e f g h\n\n");
}

void checkKing(BitBoard *board) {
    for (int i = 0; i < 32; ++i) {
        int r = i / 4;
        if (GetBit(board->redPieces, i) && r == 7) board->kings = SetBit(board->kings, i);
        if (GetBit(board->blackPieces, i) && r == 0) board->kings = SetBit(board->kings, i);
    }
}

bool canCapture(BitBoard *board, bool isRed, int idx) {
    unsigned int my = isRed ? board->redPieces : board->blackPieces;
    unsigned int opp = isRed ? board->blackPieces : board->redPieces;
    bool king = GetBit(board->kings, idx);

    int row = idx / 4;
    int col = (idx % 4) * 2 + ((row + 1) % 2);

    const int dirs[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
    for (int d = 0; d < 4; ++d) {
        int dr = dirs[d][0], dc = dirs[d][1];
        if (!king) {
            if (isRed && dr < 0) continue;    // red can only go up
            if (!isRed && dr > 0) continue;   // black can only go down
        }

        int midR = row + dr, midC = col + dc;
        int endR = row + 2*dr, endC = col + 2*dc;
        if (midR < 0 || midR >= 8 || midC < 0 || midC >= 8) continue;
        if (endR < 0 || endR >= 8 || endC < 0 || endC >= 8) continue;
        if ((midR + midC) % 2 == 0) continue;
        if ((endR + endC) % 2 == 0) continue;
        int midIdx = midR * 4 + (midC / 2);
        int endIdx = endR * 4 + (endC / 2);
        if (GetBit(opp, midIdx) && !GetBit(my, endIdx) && !GetBit(opp, endIdx))
            return true;
    }
    return false;
}

int tryMove(BitBoard *board, bool isRed, int fromIdx, int toIdx) {
    if (fromIdx < 0 || toIdx < 0 || fromIdx > 31 || toIdx > 31) return 0;

    unsigned int *my = isRed ? &board->redPieces : &board->blackPieces;
    unsigned int *opp = isRed ? &board->blackPieces : &board->redPieces;

    if (!GetBit(*my, fromIdx)) return 0;
    if (GetBit(*my, toIdx) || GetBit(*opp, toIdx)) return 0;

    int fromR = fromIdx / 4;
    int fromC = (fromIdx % 4) * 2 + ((fromR + 1) % 2);
    int toR = toIdx / 4;
    int toC = (toIdx % 4) * 2 + ((toR + 1) % 2);
    int dR = toR - fromR;
    int dC = toC - fromC;
    int absR = abs(dR), absC = abs(dC);

    if (absR != absC) return 0;

    bool king = GetBit(board->kings, fromIdx);
    int forwardDir = isRed ? 1 : -1;

    // Simple move
    if (absR == 1) {
        if (!king && dR != forwardDir) return 0;
        *my = ClearBit(*my, fromIdx);
        *my = SetBit(*my, toIdx);
        if (king) {
            board->kings = ClearBit(board->kings, fromIdx);
            board->kings = SetBit(board->kings, toIdx);
            return 1;
        }
        // Check if piece becomes a king
        if ((isRed && toR == 7) || (!isRed && toR == 0)) {
            board->kings = SetBit(board->kings, toIdx);
            return 3; // Special return value for kinging (ends turn)
        }
        return 1;
    }

    // Capture move
    if (absR == 2) {
        int midR = (fromR + toR) / 2;
        int midC = (fromC + toC) / 2;
        if ((midR + midC) % 2 == 0) return 0;
        int midIdx = midR * 4 + (midC / 2);
        if (!GetBit(*opp, midIdx)) return 0;
        if (!king && dR != forwardDir * 2) return 0; // disallow backward capture for non-king

        // Perform capture
        *opp = ClearBit(*opp, midIdx);
        *my  = ClearBit(*my, fromIdx);
        *my  = SetBit(*my, toIdx);
        if (king) {
            board->kings = ClearBit(board->kings, fromIdx);
            board->kings = SetBit(board->kings, toIdx);
            return 2;
        }
        // Check if piece becomes a king after capture
        if ((isRed && toR == 7) || (!isRed && toR == 0)) {
            board->kings = SetBit(board->kings, toIdx);
            return 4; // Special return value for kinging after capture (ends turn)
        }
        return 2;
    }

    return 0;
}

bool hasAnyMoves(BitBoard *board, bool isRed) {
    unsigned int my = isRed ? board->redPieces : board->blackPieces;
    unsigned int opp = isRed ? board->blackPieces : board->redPieces;

    for (int i = 0; i < 32; ++i) {
        if (!GetBit(my, i)) continue;

    bool king = GetBit(board->kings, i);
    int row = i / 4;
    int col = (i % 4) * 2 + ((row + 1) % 2);

        const int dirs[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };

        for (int d = 0; d < 4; ++d) {
            int dr = dirs[d][0];
            int dc = dirs[d][1];
            if (!king) {
                if (isRed && dr < 0) continue;
                if (!isRed && dr > 0) continue;
            }

            // Simple move
            int r2 = row + dr;
            int c2 = col + dc;
            if (r2 >= 0 && r2 < 8 && c2 >= 0 && c2 < 8 && ((r2 + c2) % 2 != 0)) {
                int idx2 = r2 * 4 + (c2 / 2);
                if (!GetBit(my, idx2) && !GetBit(opp, idx2)) return true;
            }

            // Capture move
            int r3 = row + 2 * dr;
            int c3 = col + 2 * dc;
            int rMid = row + dr;
            int cMid = col + dc;
            if (r3 >= 0 && r3 < 8 && c3 >= 0 && c3 < 8 && ((r3 + c3) % 2 != 0)) {
                int idx3 = r3 * 4 + (c3 / 2);
                int idxMid = rMid * 4 + (cMid / 2);
                if (GetBit(opp, idxMid) && !GetBit(my, idx3) && !GetBit(opp, idx3)) return true;
            }
        }
    }
    return false;
}

bool checkWin(BitBoard *board) {
    int redCount = CountBits(board->redPieces);
    int blackCount = CountBits(board->blackPieces);

    if (redCount == 0) {
        printf("Black wins! All red pieces captured.\n");
        return true;
    }
    if (blackCount == 0) {
        printf("Red wins! All black pieces captured.\n");
        return true;
    }

    // No legal moves available
    bool redMoves = hasAnyMoves(board, true);
    bool blackMoves = hasAnyMoves(board, false);
    if (!redMoves) {
        printf("Black wins! Red has no legal moves.\n");
        return true;
    }
    if (!blackMoves) {
        printf("Red wins! Black has no legal moves.\n");
        return true;
    }

    return false;
}

void saveGame(BitBoard *board, bool isRedTurn, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }
    fwrite(board, sizeof(BitBoard), 1, file);
    fwrite(&isRedTurn, sizeof(bool), 1, file);
    fclose(file);
    printf("Game saved to %s\n", filename);
}

bool loadGame(BitBoard *board, bool *isRedTurn, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for loading");
        return false;
    }
    fread(board, sizeof(BitBoard), 1, file);
    fread(isRedTurn, sizeof(bool), 1, file);
    fclose(file);
    printf("Game loaded from %s\n", filename);
    return true;
}

// ========================
// MAIN GAME LOOP
// ========================

int main(void) {
    bool playAgain = true;

    while (playAgain) {
        BitBoard board;
        initBoard(&board);
        printBoard(&board);

        bool isRedTurn = true;
        char inputFrom[8], inputTo[8];
        bool gameOver = false;

        char choice;
        printf("Load saved game? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            if (!loadGame(&board, &isRedTurn, "checkers_save.dat"))
                initBoard(&board);
        } else {
            initBoard(&board);
        }

        printBoard(&board);

        while (!gameOver) {
            printf("%s move (e.g. f3 e4) or 'save'/'load': ", isRedTurn ? "Red" : "Black");

            if (scanf(" %7s", inputFrom) != 1) break;

            // Allow user to type 'end' to quit the game at any time
            if (strcmp(inputFrom, "end") == 0) {
                printf("Exiting game.\n");
                gameOver = true;
                playAgain = false;
                break;
            }

            // --- SAVE OPTION ---
            if (strcmp(inputFrom, "save") == 0) {
                saveGame(&board, isRedTurn, "checkers_save.dat");
                continue;
            }

            // --- LOAD OPTION ---
            else if (strcmp(inputFrom, "load") == 0) {
                if (loadGame(&board, &isRedTurn, "checkers_save.dat"))
                    printBoard(&board);
                continue;
            }

            // Check if user entered second coordinate (or wants to end)
            else {
                if (scanf(" %7s", inputTo) != 1) {
                    printf("Error: Both FROM and TO squares are required (e.g., 'e3 f4').\n");
                    while (getchar() != '\n'); // Clear input buffer
                    continue;
                }
                if (strcmp(inputTo, "end") == 0) {
                    printf("Exiting game.\n");
                    gameOver = true;
                    playAgain = false;
                    break;
                }
            }

            // if (scanf(" %2s %2s", inputFrom, inputTo) != 2) {
            //     printf("Invalid input format. Exiting...\n");
            //     gameOver = true;
            //     break;

            // }

            inputFrom[0] = tolower((unsigned char)inputFrom[0]);
            inputTo[0]   = tolower((unsigned char)inputTo[0]);

            int fromIdx = squareToIndexStr(inputFrom);
            int toIdx   = squareToIndexStr(inputTo);
            if (fromIdx == -1 || toIdx == -1) {
                printf("Invalid square. Use format e3 f4.\n");
                continue;
            }

            int res = tryMove(&board, isRedTurn, fromIdx, toIdx);
            if (res == 0) {
                printf("Illegal move.\n");
                continue;
            }

            checkKing(&board);
            printBoard(&board);

            // Check if a piece was kinged (res == 3 or res == 4)
            if (res == 3 || res == 4) {
                printf("Piece promoted to king! Turn ends.\n");
                isRedTurn = !isRedTurn;
                continue;
            }

            // Handle multi-capture chaining
            if (res == 2) {  // Only chain captures if it was a capture move
                int cur = toIdx;
                while (canCapture(&board, isRedTurn, cur)) {
                    printf("You can capture again! Enter next landing square (e.g. c1): ");
                    if (scanf(" %7s", inputTo) != 1) {
                        gameOver = true;
                        break;
                    }
                    if (strcmp(inputTo, "end") == 0) {
                        printf("Exiting game.\n");
                        gameOver = true;
                        playAgain = false;
                        break;
                    }

                    inputTo[0] = tolower((unsigned char)inputTo[0]);
                    int nextIdx = squareToIndexStr(inputTo);
                    if (nextIdx == -1) {
                        printf("Invalid square.\n");
                        break;
                    }

                    int chainRes = tryMove(&board, isRedTurn, cur, nextIdx);
                    if (chainRes = 2) {
                        cur = nextIdx;
                        checkKing(&board);
                        printBoard(&board);
                    } else if (chainRes = 1) {
                        cur = nextIdx;
                        checkKing(&board);
                        printBoard(&board);
                        break;
                    } else {
                        printf("Invalid capture sequence.\n");
                        break;
                    }

                    if (checkWin(&board)) {
                        gameOver = true;
                        break;
                    }
                }
            }

            if (checkWin(&board)) {
                gameOver = true;
                break;
            }

            isRedTurn = !isRedTurn;
        }

        isRedTurn = !isRedTurn;

        printf("\nGame over! %s wins!\n", isRedTurn ? "Black" : "Red");

        // Ask if the user wants to play again
        char response;
        printf("Would you like to play again? (y/n): ");
        scanf(" %c", &response);

        if (response == 'y' || response == 'Y') {
            playAgain = true;  // Restart the outer loop
        } else {
            playAgain = false; // Exit loop and program
            printf("Thanks for playing!\n");
        }
    }

    return 0;
}
