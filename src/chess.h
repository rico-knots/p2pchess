#ifndef CHESS_H
#define CHESS_H

#include <stdint.h>

typedef uint64_t U64;

typedef enum {
    WHITE, BLACK
} Side;

typedef enum {
    WPAWN,
    WROOK,
    WKNIGHT,
    WBISHOP,
    WQUEEN,
    WKING,

    BPAWN,
    BROOK,
    BKNIGHT,
    BBISHOP,
    BQUEEN,
    BKING,
} Piece;

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