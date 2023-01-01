#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < lighting.frag>
#pragma qrk_include < normals.frag>

// Deferred geometry pass fragment shader.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

layout(location = 0) out vec4 gPositionAO;
layout(location = 1) out vec4 gNormalRoughness;
layout(location = 2) out vec4 gAlbedoMetallic;
layout(location = 3) out vec3 gEmission;

uniform QrkMaterial material;

void main() {
  // Fill the G-Buffer.

  // Map the fragment position and AO.
  gPositionAO.rgb = fs_in.fragPos_viewSpace;
  gPositionAO.a = qrk_extractAmbientOcclusion(material, fs_in.texCoords);
  // Lookup normal and map from tangent space to view space. Falls back to
  // vertex normal otherwise.
  gNormalRoughness.rgb =
      qrk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_viewSpace,
                    fs_in.fragNormal_viewSpace);
  gNormalRoughness.a = qrk_extractRoughness(material, fs_in.texCoords);

  gAlbedoMetallic.rgb = qrk_extractAlbedo(material, fs_in.texCoords);
  gAlbedoMetallic.a = qrk_extractMetallic(material, fs_in.texCoords);

  // We currently don't store anything in the alpha channel.
  gEmission = qrk_extractEmission(material, fs_in.texCoords);
}
