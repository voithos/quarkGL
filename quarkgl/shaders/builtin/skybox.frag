#version 460 core
out vec4 fragColor;

in vec3 skyboxCoords;

uniform samplerCube skybox;

void main() { fragColor = texture(skybox, skyboxCoords); }