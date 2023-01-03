#version 460 core
#pragma qrk_include < transforms.glsl>
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexTexCoords;

// Deferred geometry pass vertex shader.

out VS_OUT {
  vec2 texCoords;
  // TODO: Consider changing this to be worldSpace.
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

  // Propagate vertex normals in case we don't have a normal map.
  vs_out.fragNormal_viewSpace = modelViewInverseTranspose * vertexNormal;

  // Build a tangent space transform matrix.
  vec3 normal_viewSpace = normalize(vs_out.fragNormal_viewSpace);
  vec3 tangent_viewSpace =
      normalize(vec3(view * model * vec4(vertexTangent, 0.0)));
  vs_out.fragTBN_viewSpace =
      qrk_calculateTBN(normal_viewSpace, tangent_viewSpace);
}
