#include <stdint.h>

#include "./bitboard.h"
#include "chess.h"

void init_starting_position(GameState *board) {
	board->pieces[WHITE][PAWN] = RANK_2;
	board->pieces[BLACK][PAWN] = RANK_7;

	Piece back_rank_order[8] = {
		ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
	};

	for (int file = 0; file < 8; file++) {
		SET_BIT(board->pieces[WHITE][back_rank_order[file]], 0 + file);
		SET_BIT(board->pieces[BLACK][back_rank_order[file]], 56 + file);
	}
}

GameState engine_new_game() {
	GameState board = {0};

	init_starting_position(&board);

	board.white_can_castle_kingside = 1;
	board.white_can_castle_queenside = 1;
	board.black_can_castle_kingside = 1;
	board.black_can_castle_queenside = 1;

	board.en_pessant_tile = -1;
	board.halfmove_clock = 0; // Increase by 1 if half move did not capture a piece or move a pawn
	board.full_move_number = 1;
	
	time(&board.last_updated);
	return board;
}