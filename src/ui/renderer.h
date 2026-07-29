#ifndef RENDERER_H
#define RENDERER_H

#include "asset_manager.h"

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;


unsigned int get_tile_from_pos(unsigned int x, unsigned int y);
int pos_from_tile(unsigned int tile, int *x, int *y);

void renderer_init();
void draw_square(float x, float y, float w, float h, Color color);
void draw_textured_quad(float x, float y, float w, float h, GLuint tex);
void draw_sprite_from_sheet(float x, float y, float w, float h, Texture sheet, float srcX, float scrY, float scrW, float srcH);
void draw_chess_pieces(const GameState *board, int x, int y);

#endif