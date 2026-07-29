#include "./renderer.h"
#include "asset_manager.h"
#include "window.h"

GLuint quadVAO, quadVBO;

void setupQuad() {
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
    setupQuad();
}

void drawSquare(float x, float y, float w, float h, Color color) {
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

void drawTexturedQuad(float x, float y, float w, float h, GLuint tex) {
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

void drawSpriteFromSheet(float x, float y, float w, float h, Texture sheet,
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