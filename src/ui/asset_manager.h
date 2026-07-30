#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "../include/glad/gl.h"
#include <GL/glext.h>
#include <stdint.h>
#include <sys/types.h>
#include "./../engine/bitboard.h"

#include "../include/stb/stb_truetype.h"

#define ATLAS_W 512
#define ATLAS_H 512
#define FIRST_CHAR 32
#define NUM_CHARS 96

typedef struct {
	GLuint texture;
	int width;
	int height;
	int nrChannels;
} Texture;

typedef struct {
    Texture board_texture;
    Texture cursor_texture;
    Texture piece_textures[2];
} ChessTextures;

typedef struct {
	GLuint texture;
	stbtt_bakedchar char_data[NUM_CHARS];
	int atlas_width, atlas_height;
	float line_height;
} Font;

void assets_init(void);

ChessTextures *assets_get_textures(void);
Font *assets_get_font(void);
GLuint assets_get_shader_program(void);

float assets_get_piece_texture_offset(Piece piece);

#endif