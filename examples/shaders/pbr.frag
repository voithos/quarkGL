#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < normals.frag>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < standard_lights_pbr.frag>
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>

// An example fragment shader with PBR illumination.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos_viewSpace;
  vec3 fragNormal_viewSpace;
  mat3 fragTBN_viewSpace;  // Transforms from tangent frame to view frame.
}
fs_in;

uniform vec3 baseColor;
uniform float metallic;
uniform float roughness;
uniform bool usePBR;
uniform bool useTextures;

out vec4 fragColor;

uniform QrkMaterial material;

void main() {
  // Lookup normal and map from tangent space to view space.
  vec3 normal_viewSpace =
      qrk_getNormal(material, fs_in.texCoords, fs_in.fragTBN_viewSpace,
                    fs_in.fragNormal_viewSpace);

  vec3 result;
  // Shade with normal lights.
  if (usePBR) {
    if (useTextures) {
      result = qrk_shadeAllLightsCookTorranceGGX(
          material, fs_in.fragPos_viewSpace, normal_viewSpace, fs_in.texCoords);
    } else {
      // Even though we aren't using deferred shading, we can use the function
      // to pass colors directly and avoid textures.
      result = qrk_shadeAllLightsCookTorranceGGXDeferred(
          /*albedo=*/baseColor, material.ambient, roughness, metallic,
          fs_in.fragPos_viewSpace, normal_viewSpace);
    }
  } else {
    if (useTextures) {
      result = qrk_shadeAllLights(material, fs_in.fragPos_viewSpace,
                                  normal_viewSpace, fs_in.texCoords);
    } else {
      // Even though we aren't using deferred shading, we can use the function
      // to pass colors directly and avoid textures.
      result = qrk_shadeAllLightsDeferred(
          /*albedo=*/baseColor, /*specular=*/vec3(0.5), material.ambient,
          material.shininess, fs_in.fragPos_viewSpace, normal_viewSpace);
    }
  }

  // Add emissions.
  result +=
      qrk_shadeEmission(material, fs_in.fragPos_viewSpace, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));

  fragColor.rgb = qrk_toneMapReinhard(fragColor.rgb);
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
