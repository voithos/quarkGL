#version 330 core
in vec3 outputPos;
out vec4 FragColor;

void main() {
  FragColor = vec4(outputPos, 1);
}
