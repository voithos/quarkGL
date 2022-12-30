#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>

// An example fragment shader with PBR illumination.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
fs_in;

uniform vec3 baseColor;
uniform float metallic;
uniform float roughness;
uniform float usePBR;

out vec4 fragColor;

uniform QrkMaterial material;

void main() {
  vec3 normal = normalize(fs_in.fragNormal);

  // Shade with normal lights. Even though we aren't using deferred shading, we
  // can use the function to pass colors directly.
  // vec3 result =
  // qrk_shadeAllLights(material, fs_in.fragPos, normal, fs_in.texCoords);
  vec3 result = qrk_shadeAllLightsDeferred(
      /*albedo=*/baseColor, /*specular=*/vec3(0.5), material.ambient,
      material.shininess, fs_in.fragPos, normal);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));

  fragColor.rgb = qrk_toneMapReinhard(fragColor.rgb);
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
