#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SET_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))

typedef uint64_t U64;

enum squares {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

// White pieces occupy ranks 1 and 2
U64 white_pawns   = 0ULL;
U64 white_rooks   = 0ULL;
U64 white_knights = 0ULL;
U64 white_bishops = 0ULL;
U64 white_queen   = 0ULL;
U64 white_king    = 0ULL;

// Black pieces occupy ranks 7 and 8
U64 black_pawns   = 0ULL;
U64 black_rooks   = 0ULL;
U64 black_knights = 0ULL;
U64 black_bishops = 0ULL;
U64 black_queen   = 0ULL;
U64 black_king    = 0ULL;

// Set first bit, shift it by the square number to the right square
void init_starting_position() {
    // White
    white_rooks   = (1ULL << A1) | (1ULL << H1);
    white_knights = (1ULL << B1) | (1ULL << G1);
    white_bishops = (1ULL << C1) | (1ULL << F1);
    white_queen   = (1ULL << D1);
    white_king    = (1ULL << E1);
    white_pawns   = 0x000000000000FF00ULL;

    // Black
    black_rooks   = (1ULL << A8) | (1ULL << H8);
    black_knights = (1ULL << B8) | (1ULL << G8);
    black_bishops = (1ULL << C8) | (1ULL << F8);
    black_queen   = (1ULL << D8);
    black_king    = (1ULL << E8);
    black_pawns   = 0x00FF000000000000ULL;
}

// Print function
void print_board(void) {
    printf("\n  +-----------------+\n");
    
    // Loop through ranks from 8 down to 1
    for (int rank = 7; rank >= 0; rank--) {
        // Print the rank number on the left
        printf("%d | ", rank + 1);
        
        // Loop through files from A to H
        for (int file = 0; file < 8; file++) {
            // Calculate the 0-63 square index (Little-Endian Mapping)
            int square = rank * 8 + file;
            U64 bit = 1ULL << square;
            char piece = '.'; // Default empty square character

            // Check White pieces (Uppercase)
            if (white_pawns & bit)     piece = 'P';
            else if (white_rooks & bit)   piece = 'R';
            else if (white_knights & bit) piece = 'N';
            else if (white_bishops & bit) piece = 'B';
            else if (white_queen & bit)   piece = 'Q';
            else if (white_king & bit)    piece = 'K';
            
            // Check Black pieces (Lowercase)
            else if (black_pawns & bit)   piece = 'p';
            else if (black_rooks & bit)   piece = 'r';
            else if (black_knights & bit) piece = 'n';
            else if (black_bishops & bit) piece = 'b';
            else if (black_queen & bit)   piece = 'q';
            else if (black_king & bit)    piece = 'k';

            // Print the piece letter followed by a space for layout padding
            printf("%c ", piece);
        }
        printf("|\n");
    }
    
    // Print the file characters at the bottom
    printf("  +-----------------+\n");
    printf("    a b c d e f g h\n\n");
}

int main() {

    init_starting_position();
    print_board();
    
    // Keep track of whether the king may castle on either side
    int black_castle_king = 0;
    int black_castle_queen = 0;

    int white_castle_king = 0;
    int white_castle_queen = 0;

    return 0;
}