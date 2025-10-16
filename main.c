#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "BitBoard_Checkers.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    Bitboard red, black, kings;
    init_board(&red, &black, &kings);
    bool redTurn = true;
    char command[32];

    printf("=== Bitboard Checkers ===\n");
    printf("Commands:\n");
    printf("  move [from] [to] - Move a piece (e.g., 'move b6 a5')\n");
    printf("  save [filename] - Save game\n");
    printf("  load [filename] - Load game\n");
    printf("  debug - Show debug information\n");
    printf("  quit - Exit game\n");

    print_board(red, black, kings);

    while (1) {
        printf("%s's turn: ", redTurn ? "Red" : "Black");
        if (scanf("%31s", command) != 1) break;

        if (strcasecmp(command, "quit") == 0) {
            break;
        }
        else if (strcasecmp(command, "debug") == 0) {
            printf("\nDebug Information:\n");
            printf("Red pieces binary:\n");
            print_binary(red);
            printf("Black pieces binary:\n");
            print_binary(black);
            printf("Kings binary:\n");
            print_binary(kings);
            printf("\nHex representation:\n");
            printf("Red: ");   print_hex(red);
            printf("Black: "); print_hex(black);
            printf("Kings: "); print_hex(kings);
            printf("\nPiece counts:\n");
            printf("Red pieces: %d\n", count_bits(red));
            printf("Black pieces: %d\n", count_bits(black));
            printf("Kings: %d\n", count_bits(kings));
            continue;
        }
        else if (strcasecmp(command, "save") == 0) {
            char filename[256];
            if (scanf("%255s", filename) != 1) continue;
            if (save_game(filename, red, black, kings, redTurn)) {
                printf("Game saved successfully to %s\n", filename);
            } else {
                printf("Error saving game\n");
            }
            continue;
        }
        else if (strcasecmp(command, "load") == 0) {
            char filename[256];
            if (scanf("%255s", filename) != 1) continue;
            if (load_game(filename, &red, &black, &kings, &redTurn)) {
                printf("Game loaded successfully from %s\n", filename);
                print_board(red, black, kings);
            } else {
                printf("Error loading game\n");
            }
            continue;
        }
        else if (strcasecmp(command, "move") != 0) {
            printf("Unknown command. Try again.\n");
            continue;
        }

        // Handle move command
        char from[8], to[8];
        if (scanf("%7s %7s", from, to) != 2) {
            printf("Invalid move format. Use: move b6 a5\n");
            continue;
        }

        int fromSq = parse_square(from);
        int toSq = parse_square(to);

        if (fromSq < 0 || toSq < 0) {
            printf("Invalid coordinates. Try again.\n");
            continue;
        }

        Bitboard *current = redTurn ? &red : &black;
        Bitboard *enemy = redTurn ? &black : &red;

        if (try_move(current, enemy, &kings, fromSq, toSq, redTurn)) {
            print_board(red, black, kings);
            
            // Check for additional captures
            if (abs((toSq/8) - (fromSq/8)) == 2 && 
                can_capture_more(*current, *enemy, kings, toSq, redTurn)) {
                printf("Additional capture available from %s!\n", to);
                continue; // Same player's turn
            }
            
            redTurn = !redTurn;
        } else {
            printf("Illegal move. Try again.\n");
        }
    }

    printf("Game ended.\n");
    return 0;
}
