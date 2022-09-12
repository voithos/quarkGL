#version 460 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexTexCoords;

out VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);

  vs_out.texCoords = vertexTexCoords;
  vs_out.fragPos_viewSpace = vec3(view * model * vec4(vertexPos, 1.0));

  mat3 modelViewInverseTranspose = mat3(transpose(inverse(view * model)));

  // Transform vertex normal, to compare with normal mapping.
  vs_out.fragNormal_viewSpace = modelViewInverseTranspose * vertexNormal;

  // Build a tangent space transform matrix.
  vec3 T = normalize(modelViewInverseTranspose * vertexTangent);
  vec3 N = normalize(modelViewInverseTranspose * vertexNormal);
  // Make sure TBN is orthogonal.
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  vs_out.fragTBN_viewSpace = mat3(T, B, N);
}
