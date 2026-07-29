#ifndef RENDERER_H
#define RENDERER_H

#include "asset_manager.h"

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color;

void renderer_init();
void drawSquare(float x, float y, float w, float h, Color color);
void drawTexturedQuad(float x, float y, float w, float h, GLuint tex);
void drawSpriteFromSheet(float x, float y, float w, float h, Texture sheet, float srcX, float scrY, float scrW, float srcH);

#endif