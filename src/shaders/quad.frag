#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uAlpha;
uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        FragColor = texture(uTexture, TexCoord);
    } else {
        FragColor = vec4(uColor, uAlpha);
    }
}