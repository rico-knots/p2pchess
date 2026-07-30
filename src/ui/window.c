#ifndef WINDOW
#define WINDOW

#include "../include/glad/gl.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#include "./asset_manager.h"
#include "./window.h"
#include "./renderer.h"

GLFWwindow *window;
int window_width, window_height;

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

void window_draw(GameState *game_state, const char *name) {
	time_t now;
	time(&now);

	ChessTextures *tex = assets_get_textures();

	draw_textured_quad(((window_width - BOARD_SIZE) / 2.0), (window_height - BOARD_SIZE) / 2.0, BOARD_SIZE, BOARD_SIZE, tex->board_texture.texture);
	draw_chess_pieces(game_state, WHITE);

	// Draw names and timers
	draw_text(assets_get_font(), name, (window_width - BOARD_SIZE) / 2.0, (window_height + BOARD_SIZE) / 2.0 + 24, COLOR_WHITE);
	float string_width = get_string_width(assets_get_font(), "10:00");
	draw_text(assets_get_font(), "10:00", (window_width + BOARD_SIZE) / 2.0 - string_width, (window_height + BOARD_SIZE) / 2.0 + 24, COLOR_WHITE);
}

void window_end_frame() {
	// Draww cursor, in end frame so its always at the top. Could i put it at the end of window_draw? yes, would i accidently draw something else over it and never notice? probably also yes.
	ChessTextures *tex = assets_get_textures();
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);

	int px, py = 0;
	if (pos_from_tile(get_tile_from_pos(mx, my), &px, &py, 0) == 0)
		draw_square(px, py, TILE_SIZE, TILE_SIZE, (Color){0, 0, 0, 0.25});

	draw_textured_quad(mx, my, 32, 32, tex->cursor_texture.texture);

	// Draw to screen;	
	
	glfwSwapBuffers(window);
}

void window_shutdown() {
	glfwTerminate();
}

#endif