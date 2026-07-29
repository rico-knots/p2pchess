#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "../include/glad/gl.h"
#include <GL/glext.h>
#include <stdint.h>
#include <sys/types.h>
#include "./../engine/bitboard.h"

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

void assets_init(void);

ChessTextures *assets_get_textures(void);
GLuint assets_get_shader_program(void);

float assets_get_piece_texture_offset(Piece piece);

#endif