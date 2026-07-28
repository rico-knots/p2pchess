#ifndef WINDOW
#define WINDOW

#include "include/glad/gl.h"
#include "GLFW/glfw3.h"
#include <GL/glext.h>
#include <stdio.h>
#include <sys/types.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb/stb_image.h"

#define WINDOW_NAME "Chess"
#define BOARD_SIZE 640
#define TILE_SIZE (int) (BOARD_SIZE / 8)

#define PAWN_OFFSET 0
#define KNIGHT_OFFSET 16
#define ROOK_OFFSET 32
#define BISHOP_OFFSET 48
#define QUEEN_OFFSET 64
#define KING_OFFSET 80

typedef struct {
  GLuint texture;
  int width;
  int height;
  int nrChannels;
} Texture;

Texture board_texture;
Texture piece_textures[2];
Texture cursor_texture;

GLFWwindow *window;
int winWidth, winHeight;
GLuint quadVAO, quadVBO;
GLuint shaderProgram;

uint get_tile_from_pos(uint x, uint y) {
  uint xOffset = (int) x - (winWidth / 2 - BOARD_SIZE / 2);
  uint yOffset = (int) y - (winHeight / 2 - BOARD_SIZE / 2);

  if (xOffset < 0 || xOffset > BOARD_SIZE || yOffset < 0 || yOffset > BOARD_SIZE) {
    return 99;
  }

  uint rank = 7 - (int) (yOffset / TILE_SIZE);
  uint file = (int) (xOffset / TILE_SIZE);

  return 8 * rank + file;
}

int pos_from_tile(uint tile, int *x, int *y) {
  if (tile > 63) return -1;

  uint rank = tile / 8;
  uint file = tile % 8;

  *x = (winWidth / 2 - BOARD_SIZE / 2) + (int)(file * TILE_SIZE);
  *y = (winHeight / 2 - BOARD_SIZE / 2) + (int)((7 - rank) * TILE_SIZE);

  return 0;
}

void resizeCallback(GLFWwindow *window, int w, int h) {
  winWidth = w;
  winHeight = h;

  glViewport(0, 0, w, h);

  printf("Resized window to %dx%d\n", w, h);
}

char *readFile(const char *path) {
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

Texture load_texture(char *path) {
  Texture tex = {};

  glGenTextures(1, &tex.texture);
  glBindTexture(GL_TEXTURE_2D, tex.texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(1);
  
  u_char *data = stbi_load(path, &tex.width,&tex.height, &tex.nrChannels, 0);
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

GLuint compileShader(const char *path, GLenum type) {
  char *src = readFile(path);
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

GLuint createShaderProgram(const char *vertPath, const char *fragPath) {
  GLuint vert = compileShader(vertPath, GL_VERTEX_SHADER);
  GLuint frag = compileShader(fragPath, GL_FRAGMENT_SHADER);

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

void drawSquare(float x, float y, float w, float h, float r, float g, float b, float alpha) {
  glUseProgram(shaderProgram);

  glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
  glUniform2f(glGetUniformLocation(shaderProgram, "uSize"), w, h);
  glUniform2f(glGetUniformLocation(shaderProgram, "uWindowSize"),
              (float)winWidth, (float)winHeight);
  glUniform3f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b);
  glUniform1f(glGetUniformLocation(shaderProgram, "uAlpha"), alpha);
  glUniform1i(glGetUniformLocation(shaderProgram, "uUseTexture"), 0);

  glBindVertexArray(quadVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void drawTexturedQuad(float x, float y, float w, float h, GLuint tex) {
  glUseProgram(shaderProgram);

  glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
  glUniform2f(glGetUniformLocation(shaderProgram, "uSize"), w, h);
  glUniform2f(glGetUniformLocation(shaderProgram, "uWindowSize"),
              (float)winWidth, (float)winHeight);
  glUniform1i(glGetUniformLocation(shaderProgram, "uUseTexture"), 1);

  glUniform2f(glGetUniformLocation(shaderProgram, "uUVOffset"), 0.0f, 0.0f);  // full image
  glUniform2f(glGetUniformLocation(shaderProgram, "uUVSize"), 1.0f, 1.0f);    // full image

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"),
              0); // texture unit 0

  glBindVertexArray(quadVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void drawSpriteFromSheet(float x, float y, float w, float h, Texture sheet,
                          float srcX, float srcY, float srcW, float srcH) {
    glUseProgram(shaderProgram);

    glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shaderProgram, "uSize"), w, h);
    glUniform2f(glGetUniformLocation(shaderProgram, "uWindowSize"), (float)winWidth, (float)winHeight);
    glUniform1i(glGetUniformLocation(shaderProgram, "uUseTexture"), 1);

    float u0 = srcX / (float)sheet.width;
    float v0 = 1.0f - (srcY + srcH) / (float)sheet.height;
    float uw = srcW / (float)sheet.width;
    float vh = srcH / (float)sheet.height;
    glUniform2f(glGetUniformLocation(shaderProgram, "uUVOffset"), u0, v0);
    glUniform2f(glGetUniformLocation(shaderProgram, "uUVSize"), uw, vh);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sheet.texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

int init(int width, int height) {
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
  winWidth = fbWidth;
  winHeight = fbHeight;
  
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

  shaderProgram = createShaderProgram("out/shaders/quad.vert", "out/shaders/quad.frag");

  board_texture = load_texture("out/assets/board.png");
  cursor_texture = load_texture("out/assets/mouse.png");
  piece_textures[0] = load_texture("out/assets/pieces/white.png");
  piece_textures[1] = load_texture("out/assets/pieces/black.png");
  
  setupQuad();

  return 0;
}

int main() {
  if (init(1280, 720) < 0) {
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, 1);
    }

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
      get_tile_from_pos(mx, my);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    drawTexturedQuad(((float)winWidth / 2 - BOARD_SIZE / 2.0), (float)winHeight / 2 - BOARD_SIZE / 2.0, BOARD_SIZE, BOARD_SIZE, board_texture.texture);

    float drawX = (float) winWidth / 2 - BOARD_SIZE / 2.0 + 10;
    
    for (int i = 0; i < 8; i++) {
      drawSpriteFromSheet(drawX + TILE_SIZE * i, (float) winHeight / 2 - BOARD_SIZE / 2.0 + 10 + TILE_SIZE, 60, 60, piece_textures[1], PAWN_OFFSET, 0, 16, 16); 
      drawSpriteFromSheet(drawX + TILE_SIZE * i, (float) winHeight / 2 - BOARD_SIZE / 2.0 + 10 + (TILE_SIZE * 6), 60, 60, piece_textures[0], PAWN_OFFSET, 0, 16, 16);
    }
    drawSpriteFromSheet(drawX + TILE_SIZE, (float)winHeight / 2 - BOARD_SIZE / 2.0 + 10, 60, 60, piece_textures[1], KNIGHT_OFFSET, 0, 16, 16);
    drawSpriteFromSheet(drawX + TILE_SIZE * 6, (float)winHeight / 2 - BOARD_SIZE / 2.0 + 10, 60, 60, piece_textures[1], KNIGHT_OFFSET, 0, 16, 16);

    int kx, ky;
    pos_from_tile(4, &kx, &ky);
    
    drawSpriteFromSheet(kx + 10, ky + 10, 60, 60, piece_textures[0], 80, 0, 16, 16);

    int px, py = 0;
    if (pos_from_tile(get_tile_from_pos(mx, my), &px, &py) == 0)
      drawSquare(px, py, TILE_SIZE, TILE_SIZE, 0, 0, 0, 0.25);
    
    drawTexturedQuad(mx, my, 32, 32, cursor_texture.texture);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

#endif
