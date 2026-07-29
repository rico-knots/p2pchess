#ifndef WINDOW
#define WINDOW

#include "../include/glad/gl.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <sys/types.h>

#include "./asset_manager.h"
#include "./window.h"
#include "./renderer.h"

#define WINDOW_NAME "Chess"
#define BOARD_SIZE	640
#define TILE_SIZE	(int)(BOARD_SIZE / 8)

GLFWwindow *window;
int window_width, window_height;

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

void resizeCallback(GLFWwindow *windo, int w, int h) {
	window_width = w;
	window_height = h;

	glViewport(0, 0, w, h);

	printf("Resized window to %dx%d\n", w, h);
}


int window_init(int width, int height) {
	glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

	if (!glfwInit()) {
		printf("GLFW didn't initialise.\n");
		return -1;
	}

	// Window Hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, WINDOW_NAME, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // Disables v-sync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	window_width = fbWidth;
	window_height = fbHeight;

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		printf("Failed to initialise OpenGL context\n");
		return -1;
	}

	printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version),
		   GLAD_VERSION_MINOR(version));
	glViewport(0, 0, fbWidth, fbHeight);

	glfwSetFramebufferSizeCallback(window, resizeCallback);

	glClearColor(0, 0, 0, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	assets_init();
	renderer_init();

	return 0;
}

int window_should_close() {
	return glfwWindowShouldClose(window);
}

void window_begin_frame() {
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwWindowShouldClose(window);
	}

	glClear(GL_COLOR_BUFFER_BIT);
}

void window_end_frame() {
	// Drawing
	ChessTextures *tex = assets_get_textures();

	double mx, my;
	glfwGetCursorPos(window, &mx, &my);

	drawTexturedQuad(((float)window_width / 2 - BOARD_SIZE / 2.0), (float)window_height / 2 - BOARD_SIZE / 2.0, BOARD_SIZE, BOARD_SIZE, tex->board_texture.texture);

	float drawX = (float)window_width / 2 - BOARD_SIZE / 2.0 + 10;

	for (int i = 0; i < 8; i++) {
		drawSpriteFromSheet(drawX + TILE_SIZE * i, (float)window_height / 2 - BOARD_SIZE / 2.0 + 10 + TILE_SIZE, 60, 60, tex->piece_textures[1], assets_get_piece_texture_offset(PAWN), 0, 16, 16);
		drawSpriteFromSheet(drawX + TILE_SIZE * i, (float)window_height / 2 - BOARD_SIZE / 2.0 + 10 + (TILE_SIZE * 6), 60, 60, tex->piece_textures[0], assets_get_piece_texture_offset(PAWN), 0, 16, 16);
	}
	drawSpriteFromSheet(drawX + TILE_SIZE, (float)window_height / 2 - BOARD_SIZE / 2.0 + 10, 60, 60, tex->piece_textures[1], assets_get_piece_texture_offset(KNIGHT), 0, 16, 16);
	drawSpriteFromSheet(drawX + TILE_SIZE * 6, (float)window_height / 2 - BOARD_SIZE / 2.0 + 10, 60, 60, tex->piece_textures[1], assets_get_piece_texture_offset(KNIGHT), 0, 16, 16);

	int kx, ky;
	pos_from_tile(4, &kx, &ky);

	drawSpriteFromSheet(kx + 10, ky + 10, 60, 60, tex->piece_textures[0], 80, 0, 16, 16);

	int px, py = 0;
	if (pos_from_tile(get_tile_from_pos(mx, my), &px, &py) == 0)
		drawSquare(px, py, TILE_SIZE, TILE_SIZE, (Color){0, 0, 0, 0.25});

	drawTexturedQuad(mx, my, 32, 32, tex->cursor_texture.texture);

	glfwSwapBuffers(window);
}

void window_shutdown() {
	glfwTerminate();
}

// int main() {
// 	if (window_init(1280, 720) < 0) {
// 		return 1;
// 	}

// 	while (!glfwWindowShouldClose(window)) {
// 		glfwPollEvents();

// 		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
// 			glfwSetWindowShouldClose(window, 1);
// 		}

// 		double mx, my;
// 		glfwGetCursorPos(window, &mx, &my);

// 		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
// 			get_tile_from_pos(mx, my);
// 		}

// 		glClear(GL_COLOR_BUFFER_BIT);
// 		glfwSwapBuffers(window);
// 	}

// 	glfwTerminate();
// 	return 0;
// }

#endif