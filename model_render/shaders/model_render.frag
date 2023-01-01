#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < normals.frag>
#pragma qrk_include < standard_lights_phong.frag>
#pragma qrk_include < standard_lights_pbr.frag>
#pragma qrk_include < depth.frag>
#pragma qrk_include < tone_mapping.frag>

// A fragment shader for rendering models.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform int lightingModel;
uniform bool useVertexNormals;

void main() {
  vec3 normal_viewSpace;
  if (useVertexNormals) {
    normal_viewSpace = normalize(fs_in.fragNormal_viewSpace);
  } else {
    // Lookup normal and map from tangent space to view space.
    normal_viewSpace =
        qrk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_viewSpace,
                      fs_in.fragNormal_viewSpace);
  }

  vec3 result;
  // Shade with normal lights.
  if (lightingModel == 0) {
    // Phong.
    result = qrk_shadeAllLightsBlinnPhong(material, fs_in.fragPos_viewSpace,
                                          normal_viewSpace, fs_in.texCoords);
  } else if (lightingModel == 1) {
    // GGX.
    result = qrk_shadeAllLightsCookTorranceGGX(
        material, fs_in.fragPos_viewSpace, normal_viewSpace, fs_in.texCoords);
  } else {
    // Invalid lighting model.
    result = vec3(1.0, 0.0, 0.0);
  }

  // Add emissions.
  result +=
      qrk_shadeEmission(material, fs_in.fragPos_viewSpace, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_toneMapReinhard(fragColor.rgb);
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
