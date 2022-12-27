#version 460 core
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform float lod;

void main() { fragColor = textureLod(screenTexture, texCoords, lod); }
