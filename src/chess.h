#ifndef CHESS_H
#define CHESS_H

#include <stdint.h>

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

typedef struct {
    // White pieces occupy ranks 1 and 2
    U64 white_pawns;
    U64 white_rooks;
    U64 white_knights;
    U64 white_bishops;
    U64 white_queen;
    U64 white_king;

    // Black pieces occupy ranks 7 and 8
    U64 black_pawns;
    U64 black_rooks;
    U64 black_knights;
    U64 black_bishops;
    U64 black_queen;
    U64 black_king;
} ChessBoard;

void init_starting_position(ChessBoard *board);
void print_board(ChessBoard *board);

#endif