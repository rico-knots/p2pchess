#include <stdio.h>
#include <stdlib.h>
#include "../engine/bitboard.h"
#include "./asset_manager.h"
#include <sys/types.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_TRUETYPE_IMPLEMENTATION
#include "../include/stb/stb_truetype.h"
#undef STB_TRUETYPE_IMPLEMENTATION

#include "asset_manager.h"

char *board_location = "out/assets/textures/board.png";
char *cursor_location = "out/assets/textures/mouse.png";

char *piece_locations[2] = {
    "out/assets/textures/pieces/white.png",
    "out/assets/textures/pieces/black.png"
};

char *font_path = "out/assets/font.ttf";

char *vert_path = "out/assets/shaders/quad.vert";
char *frag_path = "out/assets/shaders/quad.frag";

static ChessTextures textures;
static Font font;
static GLuint shader_program;

char *read_file(const char *path) {
	FILE *f = fopen(path, "rb");
	if (!f) {
		printf("Couldnt not open %s\n", path);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	char *buf = malloc(size + 1);
	fseek(f, 0, SEEK_SET);
	fread(buf, 1, size, f);
	buf[size] = '\0';
	fclose(f);
	return buf;
}

Texture load_texture(const char *path) {
	Texture tex = {0};

	glGenTextures(1, &tex.texture);
	glBindTexture(GL_TEXTURE_2D, tex.texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(1);

	unsigned char *data = stbi_load(path, &tex.width, &tex.height, &tex.nrChannels, 0);
	if (data) {
		printf("Loaded image: %dx%d, %d channels\n", tex.width, tex.height, tex.nrChannels);
		GLenum format = (tex.nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		printf("Failed to load texture from location: %s\n", path);
	}
	fflush(stdout);

	stbi_image_free(data);

	return tex;
}

ChessTextures load_textures(void) {
    ChessTextures ct = {0};

    ct.board_texture = load_texture(board_location);
    ct.cursor_texture = load_texture(cursor_location);

    ct.piece_textures[WHITE] = load_texture(piece_locations[WHITE]);
    ct.piece_textures[BLACK] = load_texture(piece_locations[BLACK]);

    return ct;
}

Font load_font(const char *path, float pixel_height) {
	Font out_font = {0};
	out_font.atlas_width = ATLAS_W;
	out_font.atlas_height = ATLAS_H;
	out_font.line_height = pixel_height;

	char *ttf_buffer = read_file(path);
	if (!ttf_buffer) {
		printf("Failed to load font file: %s\n", font_path);
		return out_font;
	}

	unsigned char *atlas_bitmap = malloc(ATLAS_W * ATLAS_H);

	int result = stbtt_BakeFontBitmap(
		(unsigned char *)ttf_buffer,
		0, pixel_height,
		atlas_bitmap,
		ATLAS_W, ATLAS_H,
		FIRST_CHAR, NUM_CHARS,
		out_font.char_data);

	if (result <= 0)
		printf("Font baking failed or atlas too small (result: %d)\n", result);

	glGenTextures(1, &out_font.texture);
	glBindTexture(GL_TEXTURE_2D, out_font.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_W, ATLAS_H, 0, GL_RED, GL_UNSIGNED_BYTE, atlas_bitmap);

	free(atlas_bitmap);
	free(ttf_buffer);

	return out_font;
}

GLuint compile_shader(const char *path, GLenum type) {
	char *src = read_file(path);
	if (!src)
		return 0;
	GLuint shader = glad_glCreateShader(type);
	glShaderSource(shader, 1, (const char **)&src, NULL);
	glCompileShader(shader);
	free(src);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);
		printf("Shader compile error (%s): %s\n", path, log);
	}
	return shader;
}

GLuint create_shader_program(const char *vertPath, const char *fragPath) {
	GLuint vert = compile_shader(vertPath, GL_VERTEX_SHADER);
	GLuint frag = compile_shader(fragPath, GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(program, 512, NULL, log);
		printf("Shader link error: %s\n", log);
	}
	glDeleteShader(vert);
	glDeleteShader(frag);
	return program;
}

void assets_init(void) {
	textures = load_textures();
	font = load_font(font_path, 32);
	shader_program = create_shader_program(vert_path, frag_path);
}

ChessTextures *assets_get_textures(void) {
	return &textures;
}

Font *assets_get_font(void) {
	return &font;
}

GLuint assets_get_shader_program(void) {
	return shader_program;
}

float assets_get_piece_texture_offset(Piece piece) {
    return piece * 16.0f;
}