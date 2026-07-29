#include "./renderer.h"
#include "asset_manager.h"
#include "window.h"

GLuint quadVAO, quadVBO;

unsigned int get_tile_from_pos(unsigned int x, unsigned int y) {
	unsigned int xOffset = (int)x - (window_width / 2 - BOARD_SIZE / 2);
	unsigned int yOffset = (int)y - (window_height / 2 - BOARD_SIZE / 2);

	if (xOffset > BOARD_SIZE || yOffset > BOARD_SIZE) {
		return 99;
	}

	unsigned int rank = 7 - (int)(yOffset / TILE_SIZE);
	unsigned int file = (int)(xOffset / TILE_SIZE);

	return 8 * rank + file;
}

int pos_from_tile(unsigned int tile, int *x, int *y) {
	if (tile > 63)
		return -1;

	unsigned int rank = tile / 8;
	unsigned int file = tile % 8;

	*x = (window_width / 2 - BOARD_SIZE / 2) + (int)(file * TILE_SIZE);
	*y = (window_height / 2 - BOARD_SIZE / 2) + (int)((7 - rank) * TILE_SIZE);

	return 0;
}

void setup_quad() {
	// x, y, u, v
	float vertices[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

	unsigned int indices[] = {0, 1, 2, 0, 2, 3};

	GLuint ebo;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &ebo);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
				 GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
						  (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void renderer_init() {
    setup_quad();
}

void draw_square(float x, float y, float w, float h, Color color) {
    GLuint shader_program = assets_get_shader_program();
	glUseProgram(shader_program);

	glUniform2f(glGetUniformLocation(shader_program, "uPos"), x, y);
	glUniform2f(glGetUniformLocation(shader_program, "uSize"), w, h);
	glUniform2f(glGetUniformLocation(shader_program, "uWindowSize"),
				(float)window_width, (float)window_height);
	glUniform3f(glGetUniformLocation(shader_program, "uColor"), color.r, color.g, color.b);
	glUniform1f(glGetUniformLocation(shader_program, "uAlpha"), color.a);
	glUniform1i(glGetUniformLocation(shader_program, "uUseTexture"), 0);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void draw_textured_quad(float x, float y, float w, float h, GLuint tex) {
    GLuint shader_program = assets_get_shader_program();
	glUseProgram(shader_program);

	glUniform2f(glGetUniformLocation(shader_program, "uPos"), x, y);
	glUniform2f(glGetUniformLocation(shader_program, "uSize"), w, h);
	glUniform2f(glGetUniformLocation(shader_program, "uWindowSize"),
				(float)window_width, (float)window_height);
	glUniform1i(glGetUniformLocation(shader_program, "uUseTexture"), 1);

	glUniform2f(glGetUniformLocation(shader_program, "uUVOffset"), 0.0f, 0.0f); // full image
	glUniform2f(glGetUniformLocation(shader_program, "uUVSize"), 1.0f, 1.0f);   // full image

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(shader_program, "uTexture"),
				0); // texture unit 0

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void draw_sprite_from_sheet(float x, float y, float w, float h, Texture sheet,
						 float srcX, float srcY, float srcW, float srcH) {
    GLuint shader_program = assets_get_shader_program();
	glUseProgram(shader_program);

	glUniform2f(glGetUniformLocation(shader_program, "uPos"), x, y);
	glUniform2f(glGetUniformLocation(shader_program, "uSize"), w, h);
	glUniform2f(glGetUniformLocation(shader_program, "uWindowSize"), (float)window_width, (float)window_height);
	glUniform1i(glGetUniformLocation(shader_program, "uUseTexture"), 1);

	float u0 = srcX / (float)sheet.width;
	float v0 = 1.0f - (srcY + srcH) / (float)sheet.height;
	float uw = srcW / (float)sheet.width;
	float vh = srcH / (float)sheet.height;
	glUniform2f(glGetUniformLocation(shader_program, "uUVOffset"), u0, v0);
	glUniform2f(glGetUniformLocation(shader_program, "uUVSize"), uw, vh);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sheet.texture);
	glUniform1i(glGetUniformLocation(shader_program, "uTexture"), 0);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void draw_chess_pieces(const GameState *board, int x, int y) {
	ChessTextures *textures = assets_get_textures();

	int squares[64];
	int count, tileX, tileY;
	float offsetX;
	
	for (int i = 0; i < 12; i++) {
		Side side = (int)(i / 6);
		Piece piece = (int)(i % 6);

		BitBoard bb = board->pieces[side][piece];
		Texture tex = textures->piece_textures[side];
		offsetX = assets_get_piece_texture_offset(piece);

		get_squares(bb, squares, &count);

		for (int j = 0; j < count; j++) {
			int square = squares[j];
			pos_from_tile(square, &tileX, &tileY);
			draw_sprite_from_sheet(tileX + 10, tileY + 10, TILE_SIZE - 20, TILE_SIZE - 20, tex, offsetX, 0, 16, 16);	
		}
		
	}
}