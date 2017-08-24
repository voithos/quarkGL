#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 worldLightPos;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);
  fragPos = vec3(view * model * vec4(vertexPos, 1.0));
  fragNormal = mat3(transpose(inverse(view * model))) * vertexNormal;
  lightPos = vec3(view * vec4(worldLightPos, 1.0));
}
