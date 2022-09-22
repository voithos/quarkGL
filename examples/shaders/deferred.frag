#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < lighting.frag>
#pragma qrk_include < normals.frag>

// An example fragment shader for deferred rendering, into a G-Buffer.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_worldSpace;
  vec3 fragNormal_worldSpace;
  mat3 fragTBN_worldSpace;  // Transforms from tangent frame to world frame.
}
fs_in;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpecular;
layout(location = 3) out vec3 gEmission;

uniform QrkMaterial material;

void main() {
  // Fill the G-Buffer.

  // Map the fragment position.
  gPosition = fs_in.fragPos_worldSpace;
  // Lookup normal and map from tangent space to view space. Falls back to
  // vertex normal otherwise.
  gNormal = qrk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_worldSpace,
                          fs_in.fragNormal_worldSpace);

  gAlbedoSpecular.rgb = qrk_sumDiffuseColor(material, fs_in.texCoords);
  // Use a single channel for specularity. This won't support channel-specific
  // specularity.
  gAlbedoSpecular.a = qrk_sumSpecularColor(material, fs_in.texCoords).r;

  gEmission = qrk_sumEmissionColor(material, fs_in.texCoords);
}
