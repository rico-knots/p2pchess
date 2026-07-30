#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 uPos;
uniform vec2 uSize;
uniform vec2 uWindowSize;
uniform vec2 uUVOffset;  // e.g. (0.0, 0.0) for top-left of sheet
uniform vec2 uUVSize;    // e.g. (16.0/sheetW, 16.0/sheetH)

out vec2 TexCoord;

void main() {
    vec2 pixelPos = uPos + aPos * uSize;
    vec2 ndc = (pixelPos / uWindowSize) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);

    TexCoord = uUVOffset + aTexCoord * uUVSize;
}