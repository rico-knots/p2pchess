#include <stdint.h>
#include <stdio.h>
#include <stdint.h>

#include "chess.h"

#define SET_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))

typedef uint64_t U64;

// Set first bit, shift it by the square number to the right square
void init_starting_position(ChessBoard *board) {
    // White
    board->white_rooks   = (1ULL << A1) | (1ULL << H1);
    board->white_knights = (1ULL << B1) | (1ULL << G1);
    board->white_bishops = (1ULL << C1) | (1ULL << F1);
    board->white_queen   = (1ULL << D1);
    board->white_king    = (1ULL << E1);
    board->white_pawns   = 0x000000000000FF00ULL;

    // Black
    board->black_rooks   = (1ULL << A8) | (1ULL << H8);
    board->black_knights = (1ULL << B8) | (1ULL << G8);
    board->black_bishops = (1ULL << C8) | (1ULL << F8);
    board->black_queen   = (1ULL << D8);
    board->black_king    = (1ULL << E8);
    board->black_pawns   = 0x00FF000000000000ULL;
}

// Print function
void print_board(ChessBoard *board) {
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
            if (board->white_pawns & bit)     piece = 'P';
            else if (board->white_rooks & bit)   piece = 'R';
            else if (board->white_knights & bit) piece = 'N';
            else if (board->white_bishops & bit) piece = 'B';
            else if (board->white_queen & bit)   piece = 'Q';
            else if (board->white_king & bit)    piece = 'K';
            
            // Check Black pieces (Lowercase)
            else if (board->black_pawns & bit)   piece = 'p';
            else if (board->black_rooks & bit)   piece = 'r';
            else if (board->black_knights & bit) piece = 'n';
            else if (board->black_bishops & bit) piece = 'b';
            else if (board->black_queen & bit)   piece = 'q';
            else if (board->black_king & bit)    piece = 'k';

            // Print the piece letter followed by a space for layout padding
            printf("%c ", piece);
        }
        printf("|\n");
    }
    
    // Print the file characters at the bottom
    printf("  +-----------------+\n");
    printf("    a b c d e f g h\n\n");
}

// int main() {

//     ChessBoard *board = malloc(sizeof(ChessBoard));  // board now points to real memory

//     init_starting_position(board);
//     print_board(board);

//     free(board);
    
//     // Keep track of whether the king may castle on either side
//     int black_castle_king = 0;
//     int black_castle_queen = 0;

//     int white_castle_king = 0;
//     int white_castle_queen = 0;

//     return 0;
// }