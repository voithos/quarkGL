#version 460 core
layout(location = 0) in vec3 vertexPos;

out vec3 cubemapCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
  // No model transform needed for an equirect projection.
  gl_Position = projection * view * vec4(vertexPos, 1.0);
  // The sample coordinates are equivalent to the interpolated vertex positions.
  cubemapCoords = vertexPos;
}