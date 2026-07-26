#include "bitboard.h"

static inline int pop_lsb(U64 *bb) {
	int sq = __builtin_ctzll(*bb); // index of lowest set bit
	*bb &= *bb - 1;				   // clear that bit
	return sq;
}

void get_squares(U64 bb, int *squares, int *count) {
	*count = 0;
	while (bb) {
		squares[(*count)++] = pop_lsb(&bb);
	}
}

void board_to_array(ChessBoard board, U64 *buf) {
	buf[WPAWN] = board.white_pawns;
	buf[WROOK] = board.white_rooks;
	buf[WKNIGHT] = board.white_knights;
	buf[WBISHOP] = board.white_bishops;
	buf[WQUEEN] = board.white_queen;
	buf[WKING] = board.white_king;

	buf[BPAWN] = board.black_pawns;
	buf[BROOK] = board.black_rooks;
	buf[BKNIGHT] = board.black_knights;
	buf[BBISHOP] = board.black_bishops;
	buf[BQUEEN] = board.black_queen;
	buf[BKING] = board.black_king;
}

Piece get_piece(ChessBoard board, int square) {
	U64 boards[12];
	board_to_array(board, boards);

	for (int i = 0; i < 12; i++) {
		if (get_bit(boards[i], square))
			return i;
	}
	return -1;
}