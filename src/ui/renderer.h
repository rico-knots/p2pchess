#ifndef RENDERER_H
#define RENDERER_H

#include "asset_manager.h"

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

#define COLOR_WHITE (Color){1, 1, 1, 1}
#define COLOR_BLACK (Color){0, 0, 0, 1}

unsigned int get_tile_from_pos(unsigned int x, unsigned int y);
int pos_from_tile(unsigned int tile, int *x, int *y, int flipped);

void renderer_init();
void draw_square(float x, float y, float w, float h, Color color);
void draw_textured_quad(float x, float y, float w, float h, GLuint tex);
void draw_sprite_from_sheet(float x, float y, float w, float h, Texture sheet, float srcX, float scrY, float scrW, float srcH);
void draw_chess_pieces(const GameState *board, Side your_side);
float get_string_width(Font *font, const char *text);
void draw_text(Font *font, const char *text, float x, float y, Color color);

#endif