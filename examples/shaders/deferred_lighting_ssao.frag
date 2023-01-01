#version 460 core
#define QRK_MAX_POINT_LIGHTS 32
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < lighting.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D gPositionAO;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoMetallic;
uniform sampler2D gEmission;
uniform sampler2D qrk_ssao;

uniform vec3 ambient;
uniform float shininess;
uniform QrkAttenuation emissionAttenuation;

uniform bool useSsao;

void main() {
  // Extract G-Buffer for Blinn-Phong shading.
  vec3 fragPos_viewSpace = texture(gPositionAO, texCoords).rgb;
  vec3 fragNormal_viewSpace = texture(gNormalRoughness, texCoords).rgb;
  vec3 fragAlbedo = texture(gAlbedoMetallic, texCoords).rgb;
  vec3 fragSpecular = vec3(texture(gAlbedoMetallic, texCoords).a);
  vec3 fragEmission = texture(gEmission, texCoords).rgb;
  float fragAmbientOcclusion = texture(qrk_ssao, texCoords).r;

  if (!useSsao) {
    fragAmbientOcclusion = 1.0;
  }

  // Shade with normal lights.
  vec3 color = qrk_shadeAllLightsDeferred(
      fragAlbedo, fragSpecular, ambient, shininess, fragPos_viewSpace,
      fragNormal_viewSpace, /*shadow=*/0.0, fragAmbientOcclusion);

  // Add emissions.
  color += qrk_shadeEmissionDeferred(fragEmission, fragPos_viewSpace,
                                     emissionAttenuation);

  color = qrk_toneMapAcesApprox(color);
  color = qrk_gammaCorrect(color);
  fragColor = vec4(color, 1.0);
}
