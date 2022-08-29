#version 460 core
layout(location = 0) in vec3 vertexPos;

uniform mat4 model;
uniform mat4 lightViewProjection;

void main() {
  gl_Position = lightViewProjection * model * vec4(vertexPos, 1.0);
}
