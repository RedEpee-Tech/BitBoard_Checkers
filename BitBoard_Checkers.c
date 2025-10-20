
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <string.h>
// #include <ctype.h>

// //bitboard
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// //Check for dark square
// bool is_dark_square(int index) {
//     int r = index / 8, c = index % 8;
//     return (r + c) % 2 == 1;
// }

// //Board Initialization
// void init_board(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
//     *red = *black = *redKings = *blackKings = 0;

//     // Black pieces on top 3 rows
//     for (int r = 0; r < 3; r++) {
//         for (int c = 0; c < 8; c++) {
//             if ((r + c) % 2 == 1)
//                 SET_BIT(*black, r * 8 + c);
//         }
//     }

//     // Red pieces on bottom 3 rows
//     for (int r = 5; r < 8; r++) {
//         for (int c = 0; c < 8; c++) {
//             if ((r + c) % 2 == 1)
//                 SET_BIT(*red, r * 8 + c);
//         }
//     }
// }

// //Square Parser
// int parse_square(const char *s) {
//     if (strlen(s) != 2) return -1;
//     char file = tolower(s[0]);
//     char rank = s[1];

//     if (file < 'a' || file > 'h') return -1;
//     if (rank < '1' || rank > '8') return -1;

//     int col = file - 'a';
//     int row = 8 - (rank - '0');
//     return row * 8 + col;
// }

// // Board Printer
// void print_board(Bitboard red, Bitboard black, Bitboard redKings, Bitboard blackKings) {
//     printf("\n   a  b  c  d  e  f  g  h\n");
//     for (int r = 0; r < 8; r++) {
//         printf("%d ", 8 - r);
//         for (int c = 0; c < 8; c++) {
//             int i = r * 8 + c;

//             if ((r + c) % 2 == 0) {
//                 printf(" X ");
//                 continue;
//             }

//             if (CHECK_BIT(red, i)) {
//                 if (CHECK_BIT(redKings, i)) printf(" R ");
//                 else printf(" r ");
//             } else if (CHECK_BIT(black, i)) {
//                 if (CHECK_BIT(blackKings, i)) printf(" B ");
//                 else printf(" b ");
//             } else {
//                 printf(" _ ");
//             }
//         }
//         printf(" %d\n", 8 - r);
//     }
//     printf("   a  b  c  d  e  f  g  h\n\n");
// }

// //King Promotion
// void check_promotion(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
//     for (int c = 0; c < 8; c++) {
//         int top = c;
//         int bottom = 7 * 8 + c;
//         if (CHECK_BIT(*red, top)) SET_BIT(*redKings, top);
//         if (CHECK_BIT(*black, bottom)) SET_BIT(*blackKings, bottom);
//     }
// }

// // ----------- Move Logic -----------
// // 0 = invalid move
// // 1 = simple move
// // 2 = capture move
// int try_move(Bitboard *player, Bitboard *opponent,
//               Bitboard *playerKings, Bitboard *opponentKings,
//               int from, int to, bool isRedTurn) {
//     if (from < 0 || from > 63 || to < 0 || to > 63) return 0;
//     if (!is_dark_square(from) || !is_dark_square(to)) return 0;
//     if (!CHECK_BIT(*player, from)) return 0;
//     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return 0;

//     int rf = from / 8, cf = from % 8;
//     int rt = to / 8, ct = to % 8;
//     int rowDiff = rt - rf;
//     int colDiff = ct - cf;
//     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
//     int absCol = (colDiff < 0) ? -colDiff : colDiff;

//     if (absRow != absCol) return 0; // must be diagonal

//     bool isKing = CHECK_BIT(*playerKings, from);
//     int forward = isRedTurn ? -1 : 1;
//     if (isKing) forward = 0; // kings can move both ways

//     // --- Simple move ---
//     if (absRow == 1) {
//         if (!isKing && rowDiff != forward) return 0;
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return 1; // simple move
//     }

//     // --- Capture move ---
//     if (absRow == 2) {
//         int midRow = (rf + rt) / 2;
//         int midCol = (cf + ct) / 2;
//         int midSq = midRow * 8 + midCol;

//         if (!is_dark_square(midSq)) return 0;
//         if (!CHECK_BIT(*opponent, midSq)) return 0;

//         // Perform capture
//         CLEAR_BIT(*opponent, midSq);
//         CLEAR_BIT(*opponentKings, midSq);
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return 2; // capture move
//     }

//     return 0; // invalid
// }

// // ----------- Check for More Captures -----------
// bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn) {
//     int r = pos / 8, c = pos % 8;
//     bool isKing = CHECK_BIT(kings, pos);

//     int dirs[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}};
//     for (int i = 0; i < 4; i++) {
//         int dr = dirs[i][0];
//         int dc = dirs[i][1];

//         if (!isKing) {
//             if (isRedTurn && dr > 0) continue;
//             if (!isRedTurn && dr < 0) continue;
//         }

//         int nr = r + dr, nc = c + dc;
//         int mr = r + dr / 2, mc = c + dc / 2;
//         if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
//         int npos = nr * 8 + nc;
//         int mpos = mr * 8 + mc;

//         if (!CHECK_BIT(player, npos) && CHECK_BIT(opponent, mpos))
//             return true;
//     }
//     return false;
// }

// // ----------- Main Game Loop -----------
// int main(void) {
//     setvbuf(stdout, NULL, _IONBF, 0);

//     Bitboard red, black, redKings, blackKings;
//     init_board(&red, &black, &redKings, &blackKings);
//     bool redTurn = true;

//     char from[8], to[8];
//     printf("=== Bitboard Checkers ===\n");
//     printf("Enter moves like: b6 a5 or capture like: c3 e5\n");
//     printf("Type 'quit' to exit.\n");

//     print_board(red, black, redKings, blackKings);

//     while (1) {
//         printf("%s's move: ", redTurn ? "Red" : "Black");

//         if (scanf("%7s", from) != 1) break;
//         if (strcasecmp(from, "quit") == 0) break;
//         if (scanf("%7s", to) != 1) break;

//         int fromSq = parse_square(from);
//         int toSq = parse_square(to);
//         if (fromSq < 0 || toSq < 0) {
//             printf("Invalid coordinates. Try again.\n");
//             continue;
//         }

//         Bitboard *current = redTurn ? &red : &black;
//         Bitboard *enemy = redTurn ? &black : &red;
//         Bitboard *currentKings = redTurn ? &redKings : &blackKings;
//         Bitboard *enemyKings = redTurn ? &blackKings : &redKings;

//         int moveResult = try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn);

//         if (moveResult > 0) {
//             check_promotion(&red, &black, &redKings, &blackKings);
//             print_board(red, black, redKings, blackKings);

//             // Only check for more captures if this was a capture move
//             if (moveResult == 2) {

//                 bool moreCaptures = can_capture_more(*current, *enemy, *currentKings, toSq, redTurn);//

//                 int curSq = toSq;
//                 // while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                 //     printf("You can capture again! Enter next move: ");
//                 //     if (scanf("%7s", to) != 1) break;
//                 //     int nextSq = parse_square(to);
//                 //     int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);
//                 //     if (chainResult > 1) {
//                 //         check_promotion(&red, &black, &redKings, &blackKings);
//                 //         print_board(red, black, redKings, blackKings);
//                 //         curSq = nextSq;
//                 //     } else if(chainResult == 1) {
//                 //         printf("Invalid capture sequence.\n");
//                 //         break;

//                 //     }

//                 // }
//                 while (moreCaptures) {
//                     if (!can_capture_more(*current, *enemy, *currentKings, curSq, redTurn))
//                         break;

//                     printf("You can capture again! Enter next move: ");
//                     if (scanf("%7s", to) != 1) break;

//                     int nextSq = parse_square(to);
//                     int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);

//                     if (chainResult == 2) {
//                         check_promotion(&red, &black, &redKings, &blackKings);
//                         print_board(red, black, redKings, blackKings);
//                         curSq = nextSq;
//                     } else if(!can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                         //printf("Invalid capture sequence.\n");
//                         moreCaptures = false;
//                     }
//                     else if(chainResult == 1) {
//                         printf("Invalid capture sequence.\n");
//                         moreCaptures = false;
//                     }
//                 }
//             }

//             redTurn = !redTurn;
//         } else {
//             printf("Illegal move. Try again.\n");
//         }
//     }

//     printf("Game ended.\n");
//     return 0;
// }
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>

// // ========================
// // BIT MANIPULATION HELPERS
// // ========================

// unsigned int SetBit(unsigned int value, int position) {
//     return value | (1U << position);
// }

// unsigned int ClearBit(unsigned int value, int position) {
//     return value & ~(1U << position);
// }

// unsigned int ToggleBit(unsigned int value, int position) {
//     return value ^ (1U << position);
// }

// int GetBit(unsigned int value, int position) {
//     return (value >> position) & 1U;
// }

// int CountBits(unsigned int value) {
//     int count = 0;
//     while (value) {
//         count += value & 1U;
//         value >>= 1;
//     }
//     return count;
// }

// unsigned int ShiftLeft(unsigned int value, int positions) {
//     return value << positions;
// }

// unsigned int ShiftRight(unsigned int value, int positions) {
//     return value >> positions;
// }

// void PrintBinary(unsigned int value) {
//     for (int i = 31; i >= 0; i--) {
//         printf("%d", GetBit(value, i));
//         if (i % 8 == 0 && i != 0) printf(" ");
//     }
//     printf("\n");
// }

// void PrintHex(unsigned int value) {
//     printf("0x%X\n", value);
// }

// // ========================
// // CHECKERS BITBOARD LOGIC
// // ========================

// #define BOARD_SIZE 8
// #define RED_PIECE   1
// #define BLACK_PIECE 2
// #define RED_KING    3
// #define BLACK_KING  4

// typedef struct {
//     unsigned int redPieces;
//     unsigned int blackPieces;
//     unsigned int kings;
// } BitBoard;

// // Map board positions a1–h8 → 0–31 (dark squares only)
// int squareToIndex(char file, char rank) {
//     int col = file - 'a';
//     int row = rank - '1';
//     if ((row + col) % 2 == 0) return -1;
//     return (row * 4) + (col / 2);
// }

// // Initialize starting positions
// void initBoard(BitBoard *board) {
//     board->redPieces   = 0x00000FFF; // bottom 3 rows dark squares
//     board->blackPieces = 0xFFF00000; // top 3 rows dark squares
//     board->kings       = 0;
// }

// // Print board to terminal
// void printBoard(BitBoard *board) {
//     printf("\n   a b c d e f g h\n");
//     for (int row = BOARD_SIZE - 1; row >= 0; row--) {
//         printf("%d  ", row + 1);
//         for (int col = 0; col < BOARD_SIZE; col++) {
//             if ((row + col) % 2 == 0) {
//                 printf("X "); // unusable square
//                 continue;
//             }
//             int index = (row * 4) + (col / 2);
//             char piece = '_';
//             if (GetBit(board->redPieces, index)) piece = GetBit(board->kings, index) ? 'R' : 'r';
//             else if (GetBit(board->blackPieces, index)) piece = GetBit(board->kings, index) ? 'B' : 'b';
//             printf("%c ", piece);
//         }
//         printf(" %d\n", row + 1);
//     }
//     printf("   a b c d e f g h\n\n");
// }

// bool tryMove(BitBoard *board, bool isRed, int from, int to) {
//     unsigned int *myPieces   = isRed ? &board->redPieces : &board->blackPieces;
//     unsigned int *enemyPieces = isRed ? &board->blackPieces : &board->redPieces;

//     if (!GetBit(*myPieces, from)) {
//         printf("No piece there!\n");
//         return false;
//     }

//     int fromRow = from / 4;
//     int fromCol = (from % 4) * 2 + ((fromRow + 1) % 2);
//     int toRow = to / 4;
//     int toCol = (to % 4) * 2 + ((toRow + 1) % 2);
//     int rowDiff = toRow - fromRow;
//     int colDiff = toCol - fromCol;

//     if (GetBit(*myPieces, to) || GetBit(*enemyPieces, to)) {
//         printf("Destination occupied!\n");
//         return false;
//     }

//     bool isKing = GetBit(board->kings, from);
//     int dir = isRed ? 1 : -1;

//     // Normal move
//     if ((abs(rowDiff) == 1) && (abs(colDiff) == 1)) {
//         if (isKing || rowDiff == dir) {
//             *myPieces = ClearBit(*myPieces, from);
//             *myPieces = SetBit(*myPieces, to);
//             if (isKing) {
//                 board->kings = ClearBit(board->kings, from);
//                 board->kings = SetBit(board->kings, to);
//             }
//             return true;
//         } else return false;
//     }

//     // Capture move
//     if ((abs(rowDiff) == 2) && (abs(colDiff) == 2)) {
//         int midRow = (fromRow + toRow) / 2;
//         int midCol = (fromCol + toCol) / 2;
//         int midIndex = (midRow * 4) + (midCol / 2);
//         if (GetBit(*enemyPieces, midIndex)) {
//             *enemyPieces = ClearBit(*enemyPieces, midIndex);
//             *myPieces = ClearBit(*myPieces, from);
//             *myPieces = SetBit(*myPieces, to);
//             if (isKing) {
//                 board->kings = ClearBit(board->kings, from);
//                 board->kings = SetBit(board->kings, to);
//             }
//             return true;
//         }
//     }

//     return false;
// }

// // King promotion
// void checkKing(BitBoard *board) {
//     for (int i = 0; i < 32; i++) {
//         int row = i / 4;
//         if (GetBit(board->redPieces, i) && row == 7)
//             board->kings = SetBit(board->kings, i);
//         if (GetBit(board->blackPieces, i) && row == 0)
//             board->kings = SetBit(board->kings, i);
//     }
// }

// int main() {
//     BitBoard board;
//     initBoard(&board);
//     printBoard(&board);

//     bool isRedTurn = true;
//     char fromFile, toFile;
//     char fromRank, toRank;

//     while (true) {
//         printf("%s move (e.g. e3 f4): ", isRedTurn ? "Red" : "Black");
//         if (scanf(" %c%c %c%c", &fromFile, &fromRank, &toFile, &toRank) != 4) break;

//         int from = squareToIndex(fromFile, fromRank);
//         int to = squareToIndex(toFile, toRank);

//         if (from == -1 || to == -1) {
//             printf("Invalid square.\n");
//             continue;
//         }

//         if (tryMove(&board, isRedTurn, from, to)) {
//             checkKing(&board);
//             printBoard(&board);
//             isRedTurn = !isRedTurn;
//         } else {
//             printf("Illegal move.\n");
//         }
//     }
//     return 0;
// }
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <ctype.h>

// // ========================
// // BIT MANIPULATION HELPERS
// // ========================

// unsigned int SetBit(unsigned int value, int position) {
//     return value | (1U << position);
// }

// unsigned int ClearBit(unsigned int value, int position) {
//     return value & ~(1U << position);
// }

// unsigned int ToggleBit(unsigned int value, int position) {
//     return value ^ (1U << position);
// }

// int GetBit(unsigned int value, int position) {
//     return (value >> position) & 1U;
// }

// int CountBits(unsigned int value) {
//     int count = 0;
//     while (value) {
//         count += value & 1U;
//         value >>= 1;
//     }
//     return count;
// }

// unsigned int ShiftLeft(unsigned int value, int positions) {
//     return value << positions;
// }

// unsigned int ShiftRight(unsigned int value, int positions) {
//     return value >> positions;
// }

// void PrintBinary(unsigned int value) {
//     for (int i = 31; i >= 0; i--) {
//         printf("%d", GetBit(value, i));
//         if (i % 8 == 0 && i != 0) printf(" ");
//     }
//     printf("\n");
// }

// void PrintHex(unsigned int value) {
//     printf("0x%X\n", value);
// }

// // ========================
// // CHECKERS BITBOARD LOGIC
// // ========================

// #define BOARD_SIZE 8
// #define RED_PIECE   1
// #define BLACK_PIECE 2
// #define RED_KING    3
// #define BLACK_KING  4

// typedef struct {
//     unsigned int redPieces;
//     unsigned int blackPieces;
//     unsigned int kings;
// } BitBoard;

// // Map board positions a1–h8 → 0–31 (dark squares only)
// int squareToIndex(char file, char rank) {
//     int col = file - 'a';
//     int row = rank - '1';
//     if ((row + col) % 2 == 0) return -1;
//     return (row * 4) + (col / 2);
// }

// // Initialize starting positions
// void initBoard(BitBoard *board) {
//     board->redPieces   = 0x00000FFF; // bottom 3 rows dark squares
//     board->blackPieces = 0xFFF00000; // top 3 rows dark squares
//     board->kings       = 0;
// }

// // Print board to terminal
// void printBoard(BitBoard *board) {
//     printf("\n   a b c d e f g h\n");
//     for (int row = BOARD_SIZE - 1; row >= 0; row--) {
//         printf("%d  ", row + 1);
//         for (int col = 0; col < BOARD_SIZE; col++) {
//             if ((row + col) % 2 == 0) {
//                 printf("X "); // unusable square
//                 continue;
//             }
//             int index = (row * 4) + (col / 2);
//             char piece = '_';
//             if (GetBit(board->redPieces, index))
//                 piece = GetBit(board->kings, index) ? 'R' : 'r';
//             else if (GetBit(board->blackPieces, index))
//                 piece = GetBit(board->kings, index) ? 'B' : 'b';
//             printf("%c ", piece);
//         }
//         printf(" %d\n", row + 1);
//     }
//     printf("   a b c d e f g h\n\n");
// }

// // ==========================
// // MOVEMENT AND CAPTURE LOGIC
// // ==========================

// bool canCapture(BitBoard *board, bool isRed, int index) {
//     unsigned int myPieces   = isRed ? board->redPieces : board->blackPieces;
//     unsigned int enemyPieces = isRed ? board->blackPieces : board->redPieces;
//     bool isKing = GetBit(board->kings, index);

//     int row = index / 4;
//     int col = (index % 4) * 2 + ((row + 1) % 2);

//     int dirs[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
//     for (int i = 0; i < 4; i++) {
//         int dr = dirs[i][0];
//         int dc = dirs[i][1];
//         if (!isKing && ((isRed && dr < 0) || (!isRed && dr > 0))) continue;

//         int midRow = row + dr;
//         int midCol = col + dc;
//         int endRow = row + 2*dr;
//         int endCol = col + 2*dc;

//         if (midRow < 0 || midRow >= 8 || midCol < 0 || midCol >= 8 ||
//             endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8)
//             continue;

//         if ((midRow + midCol) % 2 == 0 || (endRow + endCol) % 2 == 0)
//             continue;

//         int midIndex = (midRow * 4) + (midCol / 2);
//         int endIndex = (endRow * 4) + (endCol / 2);

//         if (GetBit(enemyPieces, midIndex) && !GetBit(myPieces, endIndex) && !GetBit(enemyPieces, endIndex))
//             return true;
//     }
//     return false;
// }

// bool tryMove(BitBoard *board, bool isRed, int from, int to) {
//     unsigned int *myPieces   = isRed ? &board->redPieces : &board->blackPieces;
//     unsigned int *enemyPieces = isRed ? &board->blackPieces : &board->redPieces;

//     if (!GetBit(*myPieces, from)) return false;

//     int fromRow = from / 4;
//     int fromCol = (from % 4) * 2 + ((fromRow + 1) % 2);
//     int toRow = to / 4;
//     int toCol = (to % 4) * 2 + ((toRow + 1) % 2);
//     int rowDiff = toRow - fromRow;
//     int colDiff = toCol - fromCol;
//     bool isKing = GetBit(board->kings, from);
//     int dir = isRed ? 1 : -1;

//     if (GetBit(*myPieces, to) || GetBit(*enemyPieces, to)) return false;

//     // Normal move
//     if ((abs(rowDiff) == 1) && (abs(colDiff) == 1)) {
//         if (isKing || rowDiff == dir) {
//             *myPieces = ClearBit(*myPieces, from);
//             *myPieces = SetBit(*myPieces, to);
//             if (isKing) {
//                 board->kings = ClearBit(board->kings, from);
//                 board->kings = SetBit(board->kings, to);
//             }
//             return true;
//         }
//         return false;
//     }

//     // Capture move
//     if ((abs(rowDiff) == 2) && (abs(colDiff) == 2)) {
//         int midRow = (fromRow + toRow) / 2;
//         int midCol = (fromCol + toCol) / 2;
//         int midIndex = (midRow * 4) + (midCol / 2);
//         if (GetBit(*enemyPieces, midIndex)) {
//             *enemyPieces = ClearBit(*enemyPieces, midIndex);
//             *myPieces = ClearBit(*myPieces, from);
//             *myPieces = SetBit(*myPieces, to);
//             if (isKing) {
//                 board->kings = ClearBit(board->kings, from);
//                 board->kings = SetBit(board->kings, to);
//             }
//             return true;
//         }
//     }

//     return false;
// }

// // King promotion
// void checkKing(BitBoard *board) {
//     for (int i = 0; i < 32; i++) {
//         int row = i / 4;
//         if (GetBit(board->redPieces, i) && row == 7)
//             board->kings = SetBit(board->kings, i);
//         if (GetBit(board->blackPieces, i) && row == 0)
//             board->kings = SetBit(board->kings, i);
//     }
// }

// // ========================
// // MAIN GAME LOOP
// // ========================

// int main() {
//     BitBoard board;
//     initBoard(&board);
//     printBoard(&board);

//     bool isRedTurn = true;
//     char fromFile, toFile;
//     char fromRank, toRank;

//     while (true) {
//         printf("%s move (e.g. e3 f4): ", isRedTurn ? "Red" : "Black");
//         if (scanf(" %c%c %c%c", &fromFile, &fromRank, &toFile, &toRank) != 4) break;

//         fromFile = tolower(fromFile);
//         toFile = tolower(toFile);

//         int from = squareToIndex(fromFile, fromRank);
//         int to = squareToIndex(toFile, toRank);
//         if (from == -1 || to == -1) {
//             printf("Invalid square.\n");
//             continue;
//         }

//         bool moved = tryMove(&board, isRedTurn, from, to);
//         if (moved) {
//             checkKing(&board);
//             printBoard(&board);

//             // Handle multi-capture chaining
//             bool canContinue = canCapture(&board, isRedTurn, to);
//             while (canContinue) {
//                 printf("You can capture again! Enter next move: ");
//                 if (scanf(" %c%c", &toFile, &toRank) != 2) break;
//                 toFile = tolower(toFile);
//                 int nextTo = squareToIndex(toFile, toRank);
//                 if (nextTo == -1) break;
//                 if (!tryMove(&board, isRedTurn, to, nextTo)) {
//                     printf("Invalid capture sequence.\n");
//                     break;
//                 }
//                 to = nextTo;
//                 checkKing(&board);
//                 printBoard(&board);
//                 canContinue = canCapture(&board, isRedTurn, to);
//             }

//             isRedTurn = !isRedTurn;
//         } else {
//             printf("Illegal move.\n");
//         }
//     }
//     return 0;
// }
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
                printf("X ");
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
        }
        return 2;
    }

    return 0;
}

// ========================
// MAIN GAME LOOP
// ========================

int main(void) {
    BitBoard board;
    initBoard(&board);
    printBoard(&board);

    bool isRedTurn = true;
    char inputFrom[8], inputTo[8];

    while (1) {
        printf("%s move (e.g. e3 f4): ", isRedTurn ? "Red" : "Black");
        if (scanf(" %2s %2s", inputFrom, inputTo) != 2) break;

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

        // Handle multi-capture chaining
        if (res > 1) {
            int cur = toIdx;
            while (canCapture(&board, isRedTurn, cur)) {

                printf("You can capture again! Enter next landing square (e.g. c1): ");
                if (scanf(" %2s", inputTo) != 1) goto end_game;

                inputTo[0] = tolower((unsigned char)inputTo[0]);
                int nextIdx = squareToIndexStr(inputTo);
                if (nextIdx == -1) { printf("Invalid square.\n"); break; }

                int chainRes = tryMove(&board, isRedTurn, cur, nextIdx);
                if (chainRes = 2) {
                    cur = nextIdx;
                    checkKing(&board);
                    printBoard(&board);
                } 
                else if (chainRes = 1){
                    cur = nextIdx;
                    checkKing(&board);
                    printBoard(&board);
                    break;
                }

                else
                 {
                    printf("Invalid capture sequence.\n");
                    break;
                }
            }
        }

        isRedTurn = !isRedTurn;
    }

end_game:
    printf("Game ended.\n");
    return 0;
}
