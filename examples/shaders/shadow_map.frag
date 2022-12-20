#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader with shadow mapping.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
  vec4 fragPosLightSpace;
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform sampler2D shadowMap;

void main() {
  vec3 normal = normalize(fs_in.fragNormal);
  float shadowBias =
      qrk_shadowBias(0.001, 0.01, normal, qrk_directionalLights[0].direction);
  float shadow = qrk_shadow(shadowMap, fs_in.fragPosLightSpace, shadowBias);

  // Shade with normal lights.
  vec3 result = qrk_shadeAllLights(material, fs_in.fragPos, normal,
                                   fs_in.texCoords, shadow, /*ao=*/1);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
