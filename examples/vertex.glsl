#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 bTexCoord;

uniform mat4 modelViewProjection;

void main() {
  gl_Position = modelViewProjection * vec4(aPos, 1.0);
  bTexCoord = aTexCoord;
}
