#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  FragPos = vec3(view * model * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(view * model))) * aNormal;
  LightPos = vec3(view * vec4(lightPos, 1.0));
}
