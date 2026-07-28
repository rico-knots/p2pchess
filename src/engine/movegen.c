#include "./bitboard.h"
#include "./chess.c"

U64 pawn_moves(U64 pawn, Side side) {
	int double_moves = (side == WHITE && (pawn & RANK_2) > 0) || (side == BLACK && (pawn & RANK_7) > 0);
	U64 out = 0;

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

U64 knight_moves(U64 knight) {
	U64 l1 = (knight >> 1) & NOT_H_FILE;
	U64 l2 = (knight >> 2) & NOT_GH_FILE;
	U64 r1 = (knight << 1) & NOT_A_FILE;
	U64 r2 = (knight << 2) & NOT_AB_FILE;
	U64 h1 = l1 | r1;
	U64 h2 = l2 | r2;
	return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}