#version 460 core
layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec2 vertexTexCoords;

out vec2 texCoords;

void main() {
  gl_Position = vec4(vertexPos, 0.0, 1.0);

  texCoords = vertexTexCoords;
}
