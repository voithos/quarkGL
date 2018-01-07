#version 330 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexTexCoords;

out vec2 texCoords;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);

  texCoords = vertexTexCoords;
  fragPos = vec3(view * model * vec4(vertexPos, 1.0));
}
