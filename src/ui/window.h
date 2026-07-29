#ifndef WINDOW_H
#define WINDOW_H

#include "./../engine/bitboard.h"

#define WINDOW_NAME "Chess"
#define BOARD_SIZE	640
#define TILE_SIZE	(int)(BOARD_SIZE / 8)

extern int window_width, window_height;

int window_init(int width, int height);
int window_should_close();
void window_begin_frame();
void window_draw(GameState *game_state);
void window_end_frame();
void window_shutdown();

#endif