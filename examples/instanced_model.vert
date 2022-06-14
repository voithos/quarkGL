#version 460 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoords;
layout(location = 3) in mat4 instanceModel;

out VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
vs_out;

uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * instanceModel * vec4(vertexPos, 1.0);

  vs_out.texCoords = vertexTexCoords;
  vs_out.fragPos = vec3(view * instanceModel * vec4(vertexPos, 1.0));
  vs_out.fragNormal =
      mat3(transpose(inverse(view * instanceModel))) * vertexNormal;
}
