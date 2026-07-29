#include "./bitboard.h"

BitBoard pawn_moves(BitBoard pawn, Side side) {
	int double_moves = (side == WHITE && (pawn & RANK_2) > 0) || (side == BLACK && (pawn & RANK_7) > 0);
	BitBoard out = 0;

	if (side == WHITE) {
		out |= pawn << 8 | NOT_RANK_1;
		if (double_moves)
			out |= pawn << 16;
	} else {
		out |= pawn >> 8 | NOT_RANK_8;
		if (double_moves)
			out |= pawn >> 16;
	}

	return out;
}

BitBoard knight_moves(BitBoard knight) {
	BitBoard l1 = (knight >> 1) & NOT_H_FILE;
	BitBoard l2 = (knight >> 2) & NOT_GH_FILE;
	BitBoard r1 = (knight << 1) & NOT_A_FILE;
	BitBoard r2 = (knight << 2) & NOT_AB_FILE;
	BitBoard h1 = l1 | r1;
	BitBoard h2 = l2 | r2;
	return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}