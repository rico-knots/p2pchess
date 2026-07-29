#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
typedef uint64_t BitBoard;

#define SET_BIT(bitboard, square)	((bitboard) |= (1ULL << (square)))
#define GET_BIT(bitboard, square)	((bitboard) & (1ULL << (square)))
#define CLEAR_BIT(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

#define NOT_A_FILE	0xfefefefefefefefeULL
#define NOT_H_FILE	0x7f7f7f7f7f7f7f7fULL
#define NOT_AB_FILE 0xfcfcfcfcfcfcfcfcULL
#define NOT_GH_FILE 0x3f3f3f3f3f3f3f3fULL

#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

#define NOT_RANK_1 ~RANK_1
#define NOT_RANK_8 ~RANK_8

// clang-format off
enum squares {
	A1,B1,C1,D1,E1,F1,G1,H1,
	A2,B2,C2,D2,E2,F2,G2,H2,
	A3,B3,C3,D3,E3,G3,F3,H3,
	A4,B4,C4,D4,E4,F4,G4,H4,
	A5,B5,C5,D5,E5,F5,G5,H5,
	A6,B6,C6,D6,E6,F6,G6,H6,
	A7,B7,C7,D7,E7,F7,G7,H7,
	A8,B8,C8,D8,E8,F8,G8,H8
};
// clang-format off

typedef enum {
	WHITE = 0,
	BLACK = 1
} Side;

typedef enum {
	PAWN = 0,
	KNIGHT,
	ROOK,
	BISHOP,
	QUEEN,
	KING,
	NUMBER_OF_PIECES
} Piece;

typedef struct {
	Side side;
	Piece piece;
} Occupation;

typedef struct {
	BitBoard pieces[2][NUMBER_OF_PIECES];

	Side turn;

	uint8_t white_can_castle_kingside;
	uint8_t white_can_castle_queenside;
	uint8_t black_can_castle_kingside;
	uint8_t black_can_castle_queenside;

	int en_pessant_tile;
	int halfmove_clock;
	int full_move_number;
} GameState;

void board_to_array(GameState board, BitBoard *buf);
void get_squares(BitBoard bb, int *squares, int *count);  

#endif