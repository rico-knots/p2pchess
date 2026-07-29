#include "bitboard.h"

static inline int pop_lsb(BitBoard *bb) {
	int sq = __builtin_ctzll(*bb); // index of lowest set bit
	*bb &= *bb - 1;				   // clear that bit
	return sq;
}

void get_squares(BitBoard bb, int *squares, int *count) {
	*count = 0;
	while (bb) {
		squares[(*count)++] = pop_lsb(&bb);
	}
}

Occupation get_piece(GameState board, int square) {
	Occupation occ = {0};
	
	for (int i = PAWN; i < NUMBER_OF_PIECES; i++) {
		if (GET_BIT(board.pieces[WHITE][i], square)) occ.side = WHITE; occ.piece = i;
		if (GET_BIT(board.pieces[BLACK][i], square)) occ.side = BLACK; occ.piece = i;
	}
	return occ;
}