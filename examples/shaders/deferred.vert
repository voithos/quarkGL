#version 460 core
#pragma qrk_include < transforms.glsl>
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec2 vertexTexCoords;

// An example world-space vertex shader.

out VS_OUT {
  vec2 texCoords;
  vec3 fragPos_worldSpace;
  vec3 fragNormal_worldSpace;
  mat3 fragTBN_worldSpace;  // Transforms from tangent frame to world frame.
}
vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);

  vs_out.texCoords = vertexTexCoords;
  vs_out.fragPos_worldSpace = vec3(model * vec4(vertexPos, 1.0));

  mat3 modelViewInverseTranspose = mat3(transpose(inverse(model)));

  // Propagate vertex normals in case we don't have a normal map.
  vs_out.fragNormal_worldSpace = modelViewInverseTranspose * vertexNormal;

  // Build a tangent space transform matrix.
  vec3 normal_worldSpace = normalize(modelViewInverseTranspose * vertexNormal);
  vec3 tangent_worldSpace =
      normalize(modelViewInverseTranspose * vertexTangent);
  vs_out.fragTBN_worldSpace =
      qrk_calculateTBN(normal_worldSpace, tangent_worldSpace);
}
