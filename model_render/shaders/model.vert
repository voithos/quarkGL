#version 460 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexTexCoords;

// An example simple vertex shader.
// TODO: Pull this into a base shader.

out VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool inverseNormals;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);

  vs_out.texCoords = vertexTexCoords;
  vs_out.fragPos = vec3(view * model * vec4(vertexPos, 1.0));
  vs_out.fragNormal = mat3(transpose(inverse(view * model))) *
                      (inverseNormals ? -vertexNormal : vertexNormal);
}
