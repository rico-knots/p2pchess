#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 uColor;
uniform float uAlpha;
uniform sampler2D uTexture;

uniform bool uUseTexture;
uniform bool uTextMode;

void main() {
    if (uTextMode) {
        float alpha = texture(uTexture, TexCoord).r;
        FragColor = vec4(uColor, alpha);
    } else if (uUseTexture) {
        vec4 color = texture(uTexture, TexCoord);
        FragColor = texture(uTexture, TexCoord);
    } else {
        FragColor = vec4(uColor, uAlpha);
    }
}