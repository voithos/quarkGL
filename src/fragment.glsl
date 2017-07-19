#version 330 core
in vec3 bColor;
in vec2 bTexCoord;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
  FragColor = mix(texture(texture0, bTexCoord), texture(texture1, bTexCoord), 0.2);
}
