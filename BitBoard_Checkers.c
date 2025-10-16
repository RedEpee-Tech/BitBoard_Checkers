
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
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

// // ----------- Board Printing -----------
// void print_board(Bitboard red, Bitboard black, Bitboard redKings, Bitboard blackKings) {
//     printf("\n   a  b  c  d  e  f  g  h\n");
//     for (int r = 0; r < 8; r++) {
//         printf("%d ", 8 - r);
//         for (int c = 0; c < 8; c++) {
//             int i = r * 8 + c;

//             if ((r + c) % 2 == 0) {
//                 printf(" * ");
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

// // ----------- Move Logic -----------
// bool try_move(Bitboard *player, Bitboard *opponent,
//               Bitboard *playerKings, Bitboard *opponentKings,
//               int from, int to, bool isRedTurn) {
//     if (from < 0 || from > 63 || to < 0 || to > 63) return false;
//     if (!CHECK_BIT(*player, from)) return false;
//     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

//     int rf = from / 8, cf = from % 8;
//     int rt = to / 8, ct = to % 8;
//     int rowDiff = rt - rf;
//     int colDiff = ct - cf;
//     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
//     int absCol = (colDiff < 0) ? -colDiff : colDiff;

//     if (absRow != absCol) return false; // must be diagonal

//     bool isKing = CHECK_BIT(*playerKings, from);
//     int forward = isRedTurn ? -1 : 1;
//     if (isKing) forward = 0; // kings move both ways

//     // --- Simple move ---
//     if (absRow == 1) {
//         if (!isKing && rowDiff != forward) return false;
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     // --- Capture move ---
//     if (absRow == 2) {
//         int midRow = (rf + rt) / 2;
//         int midCol = (cf + ct) / 2;
//         int midSq = midRow * 8 + midCol;

//         if (!CHECK_BIT(*opponent, midSq)) return false;

//         // Perform capture
//         CLEAR_BIT(*opponent, midSq);
//         CLEAR_BIT(*opponentKings, midSq);
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     return false;
// }

// // Check if piece can make another capture
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
//         int mr = r + dr/2, mc = c + dc/2;
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

//         if (try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn)) {
//             // Check for king promotion
//             for (int c = 0; c < 8; c++) {
//                 int top = c;
//                 int bottom = 7 * 8 + c;
//                 if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
//                 if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
//             }

//             print_board(red, black, redKings, blackKings);

//             // Handle multi-capture
//             bool more = true;
//             int curSq = toSq;
//             while (more && can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                 printf("You can capture again! Enter next move: ");
//                 if (scanf("%7s", to) != 1) break;
//                 int nextSq = parse_square(to);
//                 if (try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn)) {
//                     print_board(red, black, redKings, blackKings);
//                     curSq = nextSq;
//                     for (int c = 0; c < 8; c++) {
//                         int top = c;
//                         int bottom = 7 * 8 + c;
//                         if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
//                         if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
//                     }
//                 } else {
//                     printf("Invalid capture sequence.\n");
//                     more = false;
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
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <stdbool.h>
// // #include <string.h>
// // #include <ctype.h>

// // // ----------- Bitboard Macros -----------
// // typedef unsigned long long Bitboard;
// // #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// // #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// // #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // // ----------- Board Initialization -----------
// // void init_board(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
// //     *red = *black = *redKings = *blackKings = 0;


// //     // Black pieces on top 3 rows (rows 0–2)
// //     for (int r = 0; r < 3; r++) {
// //         for (int c = 0; c < 8; c++) {
// //             if ((r + c) % 2 == 1) {
// //                 SET_BIT(*black, r * 8 + c);
// //             }
// //         }
// //     }

// //     // Red pieces on bottom 3 rows (rows 5–7)
// //     for (int r = 5; r < 8; r++) {
// //         for (int c = 0; c < 8; c++) {
// //             if ((r + c) % 2 == 1) {
// //                 SET_BIT(*red, r * 8 + c);
// //             }
// //         }
// //     }
// // }

// // // ----------- Square Parser -----------
// // int parse_square(const char *s) {
// //     if (strlen(s) != 2) return -1;
// //     char file = tolower(s[0]);  // 'a'..'h'
// //     char rank = s[1];           // '1'..'8'

// //     if (file < 'a' || file > 'h') return -1;
// //     if (rank < '1' || rank > '8') return -1;

// //     int col = file - 'a';
// //     int row = 8 - (rank - '0');  // rank 8 = row 0, rank 1 = row 7
// //     return row * 8 + col;
// // }

// // // ----------- Board Printing -----------
// // void print_board(Bitboard red, Bitboard black, Bitboard redKings, Bitboard blackKings) {
// //     printf("\n   a  b  c  d  e  f  g  h\n");
// //     for (int r = 0; r < 8; r++) {
// //         printf("%d ", 8 - r);
// //         for (int c = 0; c < 8; c++) {
// //             int i = r * 8 + c;

// //             if ((r + c) % 2 == 0) {
// //                 printf(" X ");
// //                 continue;
// //             }

// //             if (CHECK_BIT(red, i)) {
// //                 if (CHECK_BIT(redKings, i)) printf(" R ");  // Red King
// //                 else printf(" r ");
// //             } else if (CHECK_BIT(black, i)) {
// //                 if (CHECK_BIT(blackKings, i)) printf(" B ");  // Black King
// //                 else printf(" b ");
// //             } else {
// //                 printf(" _ ");
// //             }
// //         }
// //         printf(" %d\n", 8 - r);
// //     }
// //     printf("   a  b  c  d  e  f  g  h\n\n");
// // }


// // // ----------- Move Logic -----------
// // bool try_move(Bitboard *player, Bitboard *opponent, Bitboard *playerKings, Bitboard *opponentKings,int from, int to, bool isRedTurn){
// //     if (from < 0 || from > 63 || to < 0 || to > 63) return false;
// //     if (!CHECK_BIT(*player, from)) return false;
// //     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

// //     int rf = from / 8, cf = from % 8;
// //     int rt = to   / 8, ct = to   % 8;
// //     int rowDiff = rt - rf;
// //     int colDiff = ct - cf;
// //     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
// //     int absCol = (colDiff < 0) ? -colDiff : colDiff;

// //     if (absRow != absCol) return false; // must be diagonal

// //     // Red moves upward (decreasing row index), black downward
// //     bool isKing = CHECK_BIT(*playerKings, from);
// //     int forward = isRedTurn ? -1 : 1;

// //     // A king can move both up and down
// //     if (isKing) forward = 0;

// //     // --- Simple move (one diagonal step forward only) ---
// //     if (absRow == 1) {
// //         if (!isKing && rowDiff != forward) return false;
// //         CLEAR_BIT(*player, from);
// //         if (isKing) {
// //             CLEAR_BIT(*playerKings, from);
// //             SET_BIT(*playerKings, to);
// //         }
// //         SET_BIT(*player, to);
// //         return true;
// //     }


// //     // --- Capture move (two diagonal steps) ---
    
// //     if (absRow == 2) {
// //         int midRow = (rf + rt) / 2;
// //         int midCol = (cf + ct) / 2;
// //         int midSq  = midRow * 8 + midCol;

// //         if (!CHECK_BIT(*opponent, midSq)) return false;

// //         // Capture opponent’s piece (and remove from king board too)
// //         CLEAR_BIT(*opponent, midSq);
// //         CLEAR_BIT(*opponentKings, midSq);

// //         CLEAR_BIT(*player, from);
// //         if (isKing) {
// //             CLEAR_BIT(*playerKings, from);
// //             SET_BIT(*playerKings, to);
// //         }
// //         SET_BIT(*player, to);
// //         return true;
// //     }


// //     return false;
// // }

// // bool can_capture_from(Bitboard player, Bitboard opponent, int pos, bool isRedTurn) {
// //     int r = pos / 8, c = pos % 8;
// //     int forwardDir = isRedTurn ? -1 : 1;
// //     int dirs[4][2] = {
// //         {forwardDir, 1},
// //         {forwardDir, -1},
// //         {-forwardDir, 1},
// //         {-forwardDir, -1}
// //     };

// //     for (int i = 0; i < 4; i++) {
// //         int r1 = r + dirs[i][0];
// //         int c1 = c + dirs[i][1];
// //         int r2 = r + 2 * dirs[i][0];
// //         int c2 = c + 2 * dirs[i][1];

// //         // Ensure within board
// //         if (r2 < 0 || r2 > 7 || c2 < 0 || c2 > 7) continue;

// //         int midSq = r1 * 8 + c1;
// //         int destSq = r2 * 8 + c2;

// //         // Must have opponent in middle and empty landing square
// //         if (CHECK_BIT(opponent, midSq) && !CHECK_BIT(player, destSq) && !CHECK_BIT(opponent, destSq))
// //             return true;
// //     }
// //     return false;
// // }


// // // ----------- Main Game Loop -----------
// // int main(void) {
// //     setvbuf(stdout, NULL, _IONBF, 0); // flush output immediately (for VS Code)

// //     Bitboard red, black, redKings, blackKings;
// //     init_board(&red, &black, &redKings, &blackKings);

    
// //     bool redTurn = true;

// //     char from[8], to[8];
// //     printf("=== Bitboard Checkers ===\n");
// //     printf("Enter moves like: b6 a5 or capture like: b3 d5\n");
// //     printf("Type 'quit' to exit.\n");

// //     print_board(red, black);

// //     while (1) {
// //         printf("%s's move: ", redTurn ? "Red" : "Black");

// //         if (scanf("%7s", from) != 1) break;
// //         if (strcasecmp(from, "quit") == 0) break;
// //         if (scanf("%7s", to) != 1) break;

// //         int fromSq = parse_square(from);
// //         int toSq   = parse_square(to);

// //         if (fromSq < 0 || toSq < 0) {
// //             printf("Invalid coordinates. Try again.\n");
// //             continue;
// //         }

// //         Bitboard *current = redTurn ? &red : &black;
// //         Bitboard *enemy   = redTurn ? &black : &red;

// //         if (try_move(current, enemy, currentKings, enemyKings,
// //              fromSq, toSq, redTurn)) {
// //     print_board(red, black, redKings, blackKings) {
// //             for (int c = 0; c < 8; c++) {
// //                 int top = c;          // row 0
// //                 int bottom = 7 * 8 + c; // row 7

// //                 // Red reaching top becomes king
// //                 if (CHECK_BIT(red, top))
// //                     SET_BIT(redKings, top);

// //                 // Black reaching bottom becomes king
// //                 if (CHECK_BIT(black, bottom))
// //                     SET_BIT(blackKings, bottom);
// // }
// //             print_board(red, black);

// //             // Check if this was a capture (distance of 2 rows)
// //             int rf = fromSq / 8, rt = toSq / 8;
// //             bool wasCapture = (rf - rt == 2 || rt - rf == 2);

// //             // Allow multiple captures from same piece
// //             if (wasCapture) {
// //             int currentPos = toSq;
// //             while (can_capture_from(*current, *enemy, currentPos, redTurn)) {
// //                 printf("You can capture again! Enter next jump (from current square): ");
// //                 char nextStr[8];
// //                 if (scanf("%7s", nextStr) != 1) break;
// //                     int nextSq = parse_square(nextStr);
// //                 if (nextSq < 0) {
// //                     printf("Invalid input.\n");
// //                     break;
// //                 }
// //                 if (!try_move(current, enemy, currentPos, nextSq, redTurn)) {
// //                     printf("Illegal follow-up move. Ending turn.\n");
// //                     break;
// //                 }
// //                 currentPos = nextSq;
// //                 print_board(red, black);
// //             }
// //         }

// //         redTurn = !redTurn;
// //         } 
// //         else {
// //             printf("Illegal move. Try again.\n");
// //         }

// //     }

// //     printf("Game ended.\n");
// //     return 0;
// // }
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <stdbool.h>
// // #include <string.h>
// // #include <ctype.h>

// // // ----------- Bitboard Macros -----------
// // typedef unsigned long long Bitboard;
// // #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// // #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// // #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // // ----------- Board Initialization -----------
// // void init_board(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
// //     *red = *black = *redKings = *blackKings = 0;

// //     // Black pieces on top 3 rows
// //     for (int r = 0; r < 3; r++) {
// //         for (int c = 0; c < 8; c++) {
// //             if ((r + c) % 2 == 1)
// //                 SET_BIT(*black, r * 8 + c);
// //         }
// //     }

// //     // Red pieces on bottom 3 rows
// //     for (int r = 5; r < 8; r++) {
// //         for (int c = 0; c < 8; c++) {
// //             if ((r + c) % 2 == 1)
// //                 SET_BIT(*red, r * 8 + c);
// //         }
// //     }
// // }

// // // ----------- Square Parser -----------
// // int parse_square(const char *s) {
// //     if (strlen(s) != 2) return -1;
// //     char file = tolower(s[0]);
// //     char rank = s[1];

// //     if (file < 'a' || file > 'h') return -1;
// //     if (rank < '1' || rank > '8') return -1;

// //     int col = file - 'a';
// //     int row = 8 - (rank - '0');
// //     return row * 8 + col;
// // }

// // // ----------- Board Printing -----------
// // void print_board(Bitboard red, Bitboard black, Bitboard redKings, Bitboard blackKings) {
// //     printf("\n   a  b  c  d  e  f  g  h\n");
// //     for (int r = 0; r < 8; r++) {
// //         printf("%d ", 8 - r);
// //         for (int c = 0; c < 8; c++) {
// //             int i = r * 8 + c;

// //             if ((r + c) % 2 == 0) {
// //                 printf(" * ");
// //                 continue;
// //             }

// //             if (CHECK_BIT(red, i)) {
// //                 if (CHECK_BIT(redKings, i)) printf(" R ");
// //                 else printf(" r ");
// //             } else if (CHECK_BIT(black, i)) {
// //                 if (CHECK_BIT(blackKings, i)) printf(" B ");
// //                 else printf(" b ");
// //             } else {
// //                 printf(" _ ");
// //             }
// //         }
// //         printf(" %d\n", 8 - r);
// //     }
// //     printf("   a  b  c  d  e  f  g  h\n\n");
// // }

// // // ----------- Move Logic -----------
// // bool try_move(Bitboard *player, Bitboard *opponent,
// //               Bitboard *playerKings, Bitboard *opponentKings,
// //               int from, int to, bool isRedTurn) {
// //     if (from < 0 || from > 63 || to < 0 || to > 63) return false;
// //     if (!CHECK_BIT(*player, from)) return false;
// //     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

// //     int rf = from / 8, cf = from % 8;
// //     int rt = to / 8, ct = to % 8;
// //     int rowDiff = rt - rf;
// //     int colDiff = ct - cf;
// //     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
// //     int absCol = (colDiff < 0) ? -colDiff : colDiff;

// //     if (absRow != absCol) return false; // must be diagonal

// //     bool isKing = CHECK_BIT(*playerKings, from);
// //     int forward = isRedTurn ? -1 : 1;
// //     if (isKing) forward = 0; // kings move both ways

// //     // --- Simple move ---
// //     if (absRow == 1) {
// //         if (!isKing && rowDiff != forward) return false;
// //         CLEAR_BIT(*player, from);
// //         if (isKing) {
// //             CLEAR_BIT(*playerKings, from);
// //             SET_BIT(*playerKings, to);
// //         }
// //         SET_BIT(*player, to);
// //         return true;
// //     }

// //     // --- Capture move ---
// //     if (absRow == 2) {
// //         int midRow = (rf + rt) / 2;
// //         int midCol = (cf + ct) / 2;
// //         int midSq = midRow * 8 + midCol;

// //         if (!CHECK_BIT(*opponent, midSq)) return false;

// //         // Perform capture
// //         CLEAR_BIT(*opponent, midSq);
// //         CLEAR_BIT(*opponentKings, midSq);
// //         CLEAR_BIT(*player, from);
// //         if (isKing) {
// //             CLEAR_BIT(*playerKings, from);
// //             SET_BIT(*playerKings, to);
// //         }
// //         SET_BIT(*player, to);
// //         return true;
// //     }

// //     return false;
// // }

// // // Check if piece can make another capture
// // bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn) {
// //     int r = pos / 8, c = pos % 8;
// //     bool isKing = CHECK_BIT(kings, pos);

// //     int dirs[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}};
// //     for (int i = 0; i < 4; i++) {
// //         int dr = dirs[i][0];
// //         int dc = dirs[i][1];

// //         if (!isKing) {
// //             if (isRedTurn && dr > 0) continue;
// //             if (!isRedTurn && dr < 0) continue;
// //         }

// //         int nr = r + dr, nc = c + dc;
// //         int mr = r + dr/2, mc = c + dc/2;
// //         if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
// //         int npos = nr * 8 + nc;
// //         int mpos = mr * 8 + mc;

// //         if (!CHECK_BIT(player, npos) && CHECK_BIT(opponent, mpos))
// //             return true;
// //     }
// //     return false;
// // }

// // // ----------- Main Game Loop -----------
// // int main(void) {
// //     setvbuf(stdout, NULL, _IONBF, 0);

// //     Bitboard red, black, redKings, blackKings;
// //     init_board(&red, &black, &redKings, &blackKings);
// //     bool redTurn = true;

// //     char from[8], to[8];
// //     printf("=== Bitboard Checkers ===\n");
// //     printf("Enter moves like: b6 a5 or capture like: c3 e5\n");
// //     printf("Type 'quit' to exit.\n");

// //     print_board(red, black, redKings, blackKings);

// //     while (1) {
// //         printf("%s's move: ", redTurn ? "Red" : "Black");

// //         if (scanf("%7s", from) != 1) break;
// //         if (strcasecmp(from, "quit") == 0) break;
// //         if (scanf("%7s", to) != 1) break;

// //         int fromSq = parse_square(from);
// //         int toSq = parse_square(to);
// //         if (fromSq < 0 || toSq < 0) {
// //             printf("Invalid coordinates. Try again.\n");
// //             continue;
// //         }

// //         Bitboard *current = redTurn ? &red : &black;
// //         Bitboard *enemy = redTurn ? &black : &red;
// //         Bitboard *currentKings = redTurn ? &redKings : &blackKings;
// //         Bitboard *enemyKings = redTurn ? &blackKings : &redKings;

// //         if (try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn)) {
// //             // Check for king promotion
// //             for (int c = 0; c < 8; c++) {
// //                 int top = c;
// //                 int bottom = 7 * 8 + c;
// //                 if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
// //                 if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
// //             }

// //             print_board(red, black, redKings, blackKings);

// //             // Handle multi-capture
// //             bool more = true;
// //             int curSq = toSq;
// //             while (more && can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
// //                 printf("You can capture again! Enter next move: ");
// //                 if (scanf("%7s", to) != 1) break;
// //                 int nextSq = parse_square(to);
// //                 if (try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn)) {
// //                     print_board(red, black, redKings, blackKings);
// //                     curSq = nextSq;
// //                     for (int c = 0; c < 8; c++) {
// //                         int top = c;
// //                         int bottom = 7 * 8 + c;
// //                         if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
// //                         if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
// //                     }
// //                 } else {
// //                     printf("Invalid capture sequence.\n");
// //                     more = false;
// //                 }
// //             }

// //             redTurn = !redTurn;
// //         } else {
// //             printf("Illegal move. Try again.\n");
// //         }
// //     }

// //     printf("Game ended.\n");
// //     return 0;
// // }
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
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

// // ----------- Board Printing -----------
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

// // ----------- Move Logic -----------
// bool try_move(Bitboard *player, Bitboard *opponent,
//               Bitboard *playerKings, Bitboard *opponentKings,
//               int from, int to, bool isRedTurn) {
//     if (from < 0 || from > 63 || to < 0 || to > 63) return false;
//     if (!CHECK_BIT(*player, from)) return false;
//     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

//     int rf = from / 8, cf = from % 8;
//     int rt = to / 8, ct = to % 8;
//     int rowDiff = rt - rf;
//     int colDiff = ct - cf;
//     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
//     int absCol = (colDiff < 0) ? -colDiff : colDiff;

//     if (absRow != absCol) return false; // must be diagonal

//     bool isKing = CHECK_BIT(*playerKings, from);
//     int forward = isRedTurn ? -1 : 1;
//     if (isKing) forward = 0; // kings move both ways

//     // --- Simple move ---
//     if (absRow == 1) {
//         if (!isKing && rowDiff != forward) return false;
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     // --- Capture move ---
//     if (absRow == 2) {
//         int midRow = (rf + rt) / 2;
//         int midCol = (cf + ct) / 2;
//         int midSq = midRow * 8 + midCol;

//         if (!CHECK_BIT(*opponent, midSq)) return false;

//         // Perform capture
//         CLEAR_BIT(*opponent, midSq);
//         CLEAR_BIT(*opponentKings, midSq);
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     return false;
// }

// // ----------- Check for Further Captures -----------
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
//         int mr = r + dr/2, mc = c + dc/2;
//         if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
//         int npos = nr * 8 + nc;
//         int mpos = mr * 8 + mc;

//         // Landing square must be empty of both sides
//         if (!CHECK_BIT(player, npos) && !CHECK_BIT(opponent, npos) && CHECK_BIT(opponent, mpos))
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

//         // Determine before move if it’s a capture (row diff == 2)
//         int rf = fromSq / 8, rt = toSq / 8;
//         int absRow = (rt > rf) ? rt - rf : rf - rt;
//         bool wasCapture = (absRow == 2);

//         if (try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn)) {
//             // King promotion
//             for (int c = 0; c < 8; c++) {
//                 int top = c;
//                 int bottom = 7 * 8 + c;
//                 if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
//                 if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
//             }

//             print_board(red, black, redKings, blackKings);

//             // Only start multi-capture if last move was an actual capture
//             if (wasCapture) {
//                 int curSq = toSq;
//                 while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                     printf("You can capture again! Enter next move: ");
//                     if (scanf("%7s", to) != 1) break;
//                     int nextSq = parse_square(to);
//                     if (nextSq < 0) { printf("Invalid square.\n"); break; }
//                     if (try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn)) {
//                         // Kinging again if needed
//                         for (int c = 0; c < 8; c++) {
//                             int top = c;
//                             int bottom = 7 * 8 + c;
//                             if (CHECK_BIT(red, top)) SET_BIT(redKings, top);
//                             if (CHECK_BIT(black, bottom)) SET_BIT(blackKings, bottom);
//                         }
//                         print_board(red, black, redKings, blackKings);
//                         curSq = nextSq;
//                     } else {
//                         printf("Invalid capture sequence.\n");
//                         break;
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
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Helper: check if usable square -----------
// static inline bool is_dark_square(int idx) {
//     int r = idx / 8, c = idx % 8;
//     return (r + c) % 2 == 1;
// }

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
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

// // ----------- Board Printing -----------
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

// // ----------- Move Logic -----------
// bool try_move(Bitboard *player, Bitboard *opponent,
//               Bitboard *playerKings, Bitboard *opponentKings,
//               int from, int to, bool isRedTurn) {
//     if (from < 0 || from > 63 || to < 0 || to > 63) return false;
//     if (!is_dark_square(from) || !is_dark_square(to)) return false;
//     if (!CHECK_BIT(*player, from)) return false;
//     if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return false;

//     int rf = from / 8, cf = from % 8;
//     int rt = to / 8, ct = to % 8;
//     int rowDiff = rt - rf;
//     int colDiff = ct - cf;
//     int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
//     int absCol = (colDiff < 0) ? -colDiff : colDiff;

//     if (absRow != absCol) return false; // must be diagonal

//     bool isKing = CHECK_BIT(*playerKings, from);
//     int forward = isRedTurn ? -1 : 1;
//     if (isKing) forward = 0; // kings move both ways

//     // --- Simple move ---
//     if (absRow == 1) {
//         if (!isKing && rowDiff != forward) return false;
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     // --- Capture move ---
//     if (absRow == 2) {
//         int midRow = (rf + rt) / 2;
//         int midCol = (cf + ct) / 2;
//         int midSq = midRow * 8 + midCol;

//         if (!is_dark_square(midSq)) return false;
//         if (!CHECK_BIT(*opponent, midSq)) return false;

//         // Perform capture
//         CLEAR_BIT(*opponent, midSq);
//         CLEAR_BIT(*opponentKings, midSq);
//         CLEAR_BIT(*player, from);
//         if (isKing) {
//             CLEAR_BIT(*playerKings, from);
//             SET_BIT(*playerKings, to);
//         }
//         SET_BIT(*player, to);
//         return true;
//     }

//     return false;
// }

// // ----------- Multi-Capture Check -----------
// bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn) {
//     int r = pos / 8, c = pos % 8;
//     bool isKing = CHECK_BIT(kings, pos);

//     int dirs[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}};
//     for (int i = 0; i < 4; i++) {
//         int dr = dirs[i][0];
//         int dc = dirs[i][1];

//         if (!isKing) {
//             if (isRedTurn && dr > 0) continue;   // red moves up
//             if (!isRedTurn && dr < 0) continue;  // black moves down
//         }

//         int nr = r + dr, nc = c + dc;
//         int mr = r + dr/2, mc = c + dc/2;

//         if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
//         if (mr < 0 || mr >= 8 || mc < 0 || mc >= 8) continue;

//         int npos = nr * 8 + nc;
//         int mpos = mr * 8 + mc;

//         if (!is_dark_square(npos) || !is_dark_square(mpos)) continue;

//         if (!CHECK_BIT(player, npos) && !CHECK_BIT(opponent, npos) && CHECK_BIT(opponent, mpos))
//             return true;
//     }
//     return false;
// }

// // Promote piece if reaching end row (works during multi-jump)
// void check_promotion(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
//     for (int c = 0; c < 8; c++) {
//         int top = c;
//         int bottom = 7 * 8 + c;
//         if (CHECK_BIT(*red, top)) SET_BIT(*redKings, top);
//         if (CHECK_BIT(*black, bottom)) SET_BIT(*blackKings, bottom);
//     }
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

//         if (try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn)) {
//             check_promotion(&red, &black, &redKings, &blackKings); // check after move

//             print_board(red, black, redKings, blackKings);

//             // Handle multi-capture
//             bool more = true;
//             int curSq = toSq;
//             while (more && can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                 printf("You can capture again! Enter next move: ");
//                 if (scanf("%7s", to) != 1) break;
//                 int nextSq = parse_square(to);
//                 if (try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn)) {
//                     check_promotion(&red, &black, &redKings, &blackKings); // promote mid-chain
//                     print_board(red, black, redKings, blackKings);
//                     curSq = nextSq;
//                 } else {
//                     printf("Invalid capture sequence.\n");
//                     more = false;
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
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Utility Functions -----------
// bool is_dark_square(int index) {
//     int r = index / 8, c = index % 8;
//     return (r + c) % 2 == 1;
// }

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
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

// // ----------- Board Printing -----------
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

// // ----------- King Promotion -----------
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
//                 int curSq = toSq;
//                 while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                     printf("You can capture again! Enter next move: ");
//                     if (scanf("%7s", to) != 1) break;
//                     int nextSq = parse_square(to);
//                     int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);
//                     if (chainResult == 2) {
//                         check_promotion(&red, &black, &redKings, &blackKings);
//                         print_board(red, black, redKings, blackKings);
//                         curSq = nextSq;
//                     } else {
//                         printf("Invalid capture sequence.\n");
//                         break;
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
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Utility Functions -----------
// bool is_dark_square(int index) {
//     int r = index / 8, c = index % 8;
//     return (r + c) % 2 == 1;
// }

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
// int parse_square(const char *s) {
//     if (strlen(s) != 2) return -1;
//     char file = tolower((unsigned char)s[0]);
//     char rank = s[1];

//     if (file < 'a' || file > 'h') return -1;
//     if (rank < '1' || rank > '8') return -1;

//     int col = file - 'a';
//     int row = 8 - (rank - '0'); // '1' -> row 7 (bottom), '8' -> row 0 (top)
//     return row * 8 + col;
// }

// // ----------- Board Printing -----------
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

// // ----------- King Promotion -----------
// void check_promotion(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
//     for (int c = 0; c < 8; c++) {
//         int top = c;             // row 0, col c
//         int bottom = 7 * 8 + c;  // row 7, col c
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

//         if (midRow < 0 || midRow >= 8 || midCol < 0 || midCol >= 8) return 0;
//         if (!is_dark_square(midSq)) return 0;
//         if (!CHECK_BIT(*opponent, midSq)) return 0; // must jump an opponent piece

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
// // 
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <string.h>
// #include <ctype.h>

// // ----------- Bitboard Macros -----------
// typedef unsigned long long Bitboard;
// #define SET_BIT(b, i)    ((b) |= (1ULL << (i)))
// #define CLEAR_BIT(b, i)  ((b) &= ~(1ULL << (i)))
// #define CHECK_BIT(b, i)  ((b) &  (1ULL << (i)))

// // ----------- Utility Functions -----------
// bool is_dark_square(int index) {
//     int r = index / 8, c = index % 8;
//     return (r + c) % 2 == 1;
// }

// // ----------- Board Initialization -----------
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

// // ----------- Square Parser -----------
// int parse_square(const char *s) {
//     if (strlen(s) != 2) return -1;
//     char file = tolower((unsigned char)s[0]);
//     char rank = s[1];

//     if (file < 'a' || file > 'h') return -1;
//     if (rank < '1' || rank > '8') return -1;

//     int col = file - 'a';
//     int row = 8 - (rank - '0'); // '1' -> row 7 (bottom), '8' -> row 0 (top)
//     return row * 8 + col;
// }

// // ----------- Board Printing -----------
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

// // ----------- King Promotion -----------
// void check_promotion(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
//     for (int c = 0; c < 8; c++) {
//         int top = c;             // row 0, col c
//         int bottom = 7 * 8 + c;  // row 7, col c
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

//         if (midRow < 0 || midRow >= 8 || midCol < 0 || midCol >= 8) return 0;
//         if (!is_dark_square(midSq)) return 0;
//         if (!CHECK_BIT(*opponent, midSq)) return 0; // must jump an opponent piece

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
//             if (isRedTurn && dr > 0) continue;  // red moves up (negative dr)
//             if (!isRedTurn && dr < 0) continue; // black moves down (positive dr)
//         }

//         int nr = r + dr, nc = c + dc;
//         int mr = r + dr / 2, mc = c + dc / 2;

//         // destination and middle must be on board
//         if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
//         if (mr < 0 || mr >= 8 || mc < 0 || mc >= 8) continue;

//         int npos = nr * 8 + nc;
//         int mpos = mr * 8 + mc;

//         // landing square must be empty (no player's piece AND no opponent piece)
//         if (CHECK_BIT(player, npos) || CHECK_BIT(opponent, npos)) continue;

//         // middle square must contain an opponent piece to jump
//         if (CHECK_BIT(opponent, mpos))
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
//                 int curSq = toSq;
//                 while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                     printf("You can capture again! Enter next move: ");
//                     if (scanf("%7s", to) != 1) break;
//                     int nextSq = parse_square(to);
//                     if (nextSq < 0) {
//                         printf("Invalid coordinates. Ending capture chain.\n");
//                         break;
//                     }
//                     int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);
//                     if (chainResult == 2) {
//                         check_promotion(&red, &black, &redKings, &blackKings);
//                         print_board(red, black, redKings, blackKings);
//                         curSq = nextSq;
//                     } else {
//                         printf("Invalid capture sequence.\n");
//                         break;
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
//                 int curSq = toSq;
//                 while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
//                     printf("You can capture again! Enter next move: ");
//                     if (scanf("%7s", to) != 1) break;
//                     int nextSq = parse_square(to);
//                     if (nextSq < 0) {
//                         printf("Invalid coordinates. Ending capture chain.\n");
//                         break;
//                     }
//                     int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);
//                     if (chainResult == 2) {
//                         check_promotion(&red, &black, &redKings, &blackKings);
//                         print_board(red, black, redKings, blackKings);
//                         curSq = nextSq;
//                     } else {
//                         printf("Invalid capture sequence.\n");
//                         break;
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

// ----------- Utility Functions -----------
bool is_dark_square(int index) {
    int r = index / 8, c = index % 8;
    return (r + c) % 2 == 1;
}

// ----------- Board Initialization -----------
void init_board(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
    *red = *black = *redKings = *blackKings = 0;

    // Black pieces on top 3 rows
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1)
                SET_BIT(*black, r * 8 + c);
        }
    }

    // Red pieces on bottom 3 rows
    for (int r = 5; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 1)
                SET_BIT(*red, r * 8 + c);
        }
    }
}

// ----------- Square Parser -----------
int parse_square(const char *s) {
    if (strlen(s) != 2) return -1;
    char file = tolower(s[0]);
    char rank = s[1];

    if (file < 'a' || file > 'h') return -1;
    if (rank < '1' || rank > '8') return -1;

    int col = file - 'a';
    int row = 8 - (rank - '0');
    return row * 8 + col;
}

// ----------- Board Printing -----------
void print_board(Bitboard red, Bitboard black, Bitboard redKings, Bitboard blackKings) {
    printf("\n   a  b  c  d  e  f  g  h\n");
    for (int r = 0; r < 8; r++) {
        printf("%d ", 8 - r);
        for (int c = 0; c < 8; c++) {
            int i = r * 8 + c;

            if ((r + c) % 2 == 0) {
                printf(" X ");
                continue;
            }

            if (CHECK_BIT(red, i)) {
                if (CHECK_BIT(redKings, i)) printf(" R ");
                else printf(" r ");
            } else if (CHECK_BIT(black, i)) {
                if (CHECK_BIT(blackKings, i)) printf(" B ");
                else printf(" b ");
            } else {
                printf(" _ ");
            }
        }
        printf(" %d\n", 8 - r);
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
}

// ----------- King Promotion -----------
void check_promotion(Bitboard *red, Bitboard *black, Bitboard *redKings, Bitboard *blackKings) {
    for (int c = 0; c < 8; c++) {
        int top = c;
        int bottom = 7 * 8 + c;
        if (CHECK_BIT(*red, top)) SET_BIT(*redKings, top);
        if (CHECK_BIT(*black, bottom)) SET_BIT(*blackKings, bottom);
    }
}

// ----------- Move Logic -----------
// 0 = invalid move
// 1 = simple move
// 2 = capture move
int try_move(Bitboard *player, Bitboard *opponent,
              Bitboard *playerKings, Bitboard *opponentKings,
              int from, int to, bool isRedTurn) {
    if (from < 0 || from > 63 || to < 0 || to > 63) return 0;
    if (!is_dark_square(from) || !is_dark_square(to)) return 0;
    if (!CHECK_BIT(*player, from)) return 0;
    if (CHECK_BIT(*player, to) || CHECK_BIT(*opponent, to)) return 0;

    int rf = from / 8, cf = from % 8;
    int rt = to / 8, ct = to % 8;
    int rowDiff = rt - rf;
    int colDiff = ct - cf;
    int absRow = (rowDiff < 0) ? -rowDiff : rowDiff;
    int absCol = (colDiff < 0) ? -colDiff : colDiff;

    if (absRow != absCol) return 0; // must be diagonal

    bool isKing = CHECK_BIT(*playerKings, from);
    int forward = isRedTurn ? -1 : 1;
    if (isKing) forward = 0; // kings can move both ways

    // --- Simple move ---
    if (absRow == 1) {
        if (!isKing && rowDiff != forward) return 0;
        CLEAR_BIT(*player, from);
        if (isKing) {
            CLEAR_BIT(*playerKings, from);
            SET_BIT(*playerKings, to);
        }
        SET_BIT(*player, to);
        return 1; // simple move
    }

    // --- Capture move ---
    if (absRow == 2) {
        int midRow = (rf + rt) / 2;
        int midCol = (cf + ct) / 2;
        int midSq = midRow * 8 + midCol;

        if (!is_dark_square(midSq)) return 0;
        if (!CHECK_BIT(*opponent, midSq)) return 0;

        // Perform capture
        CLEAR_BIT(*opponent, midSq);
        CLEAR_BIT(*opponentKings, midSq);
        CLEAR_BIT(*player, from);
        if (isKing) {
            CLEAR_BIT(*playerKings, from);
            SET_BIT(*playerKings, to);
        }
        SET_BIT(*player, to);
        return 2; // capture move
    }

    return 0; // invalid
}

// ----------- Check for More Captures -----------
bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn) {
    int r = pos / 8, c = pos % 8;
    bool isKing = CHECK_BIT(kings, pos);

    int dirs[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}};
    for (int i = 0; i < 4; i++) {
        int dr = dirs[i][0];
        int dc = dirs[i][1];

        if (!isKing) {
            if (isRedTurn && dr > 0) continue;
            if (!isRedTurn && dr < 0) continue;
        }

        int nr = r + dr, nc = c + dc;
        int mr = r + dr / 2, mc = c + dc / 2;
        if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
        int npos = nr * 8 + nc;
        int mpos = mr * 8 + mc;

        if (!CHECK_BIT(player, npos) && CHECK_BIT(opponent, mpos))
            return true;
    }
    return false;
}

// ----------- Main Game Loop -----------
int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    Bitboard red, black, redKings, blackKings;
    init_board(&red, &black, &redKings, &blackKings);
    bool redTurn = true;

    char from[8], to[8];
    printf("=== Bitboard Checkers ===\n");
    printf("Enter moves like: b6 a5 or capture like: c3 e5\n");
    printf("Type 'quit' to exit.\n");

    print_board(red, black, redKings, blackKings);

    while (1) {
        printf("%s's move: ", redTurn ? "Red" : "Black");

        if (scanf("%7s", from) != 1) break;
        if (strcasecmp(from, "quit") == 0) break;
        if (scanf("%7s", to) != 1) break;

        int fromSq = parse_square(from);
        int toSq = parse_square(to);
        if (fromSq < 0 || toSq < 0) {
            printf("Invalid coordinates. Try again.\n");
            continue;
        }

        Bitboard *current = redTurn ? &red : &black;
        Bitboard *enemy = redTurn ? &black : &red;
        Bitboard *currentKings = redTurn ? &redKings : &blackKings;
        Bitboard *enemyKings = redTurn ? &blackKings : &redKings;

        int moveResult = try_move(current, enemy, currentKings, enemyKings, fromSq, toSq, redTurn);

        if (moveResult > 0) {
            check_promotion(&red, &black, &redKings, &blackKings);
            print_board(red, black, redKings, blackKings);

            // Only check for more captures if this was a capture move
            if (moveResult == 2) {
                int curSq = toSq;
                while (can_capture_more(*current, *enemy, *currentKings, curSq, redTurn)) {
                    printf("You can capture again! Enter next move: ");
                    if (scanf("%7s", to) != 1) break;
                    int nextSq = parse_square(to);
                    int chainResult = try_move(current, enemy, currentKings, enemyKings, curSq, nextSq, redTurn);
                    if (chainResult > 1) {
                        check_promotion(&red, &black, &redKings, &blackKings);
                        print_board(red, black, redKings, blackKings);
                        curSq = nextSq;
                    } else if(chainResult == 1) {
                        printf("Invalid capture sequence.\n");
                        break;
                        
                    }
                }
            }

            redTurn = !redTurn;
        } else {
            printf("Illegal move. Try again.\n");
        }
    }

    printf("Game ended.\n");
    return 0;
}
