#ifndef BITBOARD_CHECKERS_H
#define BITBOARD_CHECKERS_H

#include <stdbool.h>

typedef unsigned long long Bitboard;

// Bitboard operations (macros kept for compatibility)
#define SET_BIT(b, i)     ((b) |= (1ULL << (i)))
#define CLEAR_BIT(b, i)   ((b) &= ~(1ULL << (i)))
#define TOGGLE_BIT(b, i)  ((b) ^= (1ULL << (i)))
#define CHECK_BIT(b, i)   ((b) & (1ULL << (i)))

// Function declarations
void init_board(Bitboard *red, Bitboard *black, Bitboard *kings);
int parse_square(const char *s);
void print_board(Bitboard red, Bitboard black, Bitboard kings);
bool try_move(Bitboard *player, Bitboard *opponent, Bitboard *kings, int from, int to, bool isRedTurn);
int count_bits(Bitboard b);
Bitboard shift_left(Bitboard b, int n);
Bitboard shift_right(Bitboard b, int n);
void print_binary(Bitboard b);
void print_hex(Bitboard b);
bool can_capture_more(Bitboard player, Bitboard opponent, Bitboard kings, int pos, bool isRedTurn);

// PascalCase helpers requested by user
void SetBit(Bitboard *b, int i);
void ClearBit(Bitboard *b, int i);
void ToggleBit(Bitboard *b, int i);
int GetBit(Bitboard b, int i);
int CountBits(Bitboard b);
Bitboard ShiftLeft(Bitboard b, int n);
Bitboard ShiftRight(Bitboard b, int n);
void PrintBinary(Bitboard b);
void PrintHex(Bitboard b);
bool save_game(const char *filename, Bitboard red, Bitboard black, Bitboard kings, bool redTurn);
bool load_game(const char *filename, Bitboard *red, Bitboard *black, Bitboard *kings, bool *redTurn);

#endif
