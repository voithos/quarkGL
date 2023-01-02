#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights_phong.frag>
#pragma qrk_include < standard_lights_pbr.frag>
#pragma qrk_include < depth.frag>
#pragma qrk_include < tone_mapping.frag>

// A fragment shader for rendering models.

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D gPositionAO;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoMetallic;
uniform sampler2D gEmission;

uniform vec3 ambient;
uniform float shininess;
uniform float emissionStrength;
uniform QrkAttenuation emissionAttenuation;

uniform int lightingModel;
// TODO: This is currently broken, needs to be supported in the geometry pass.
uniform bool useVertexNormals;

void main() {
  // Extract G-Buffer for PBR rendering.
  vec3 fragPos_viewSpace = texture(gPositionAO, texCoords).rgb;
  float fragAO = texture(gPositionAO, texCoords).a;
  vec3 fragNormal_viewSpace = texture(gNormalRoughness, texCoords).rgb;
  float fragRoughness = texture(gNormalRoughness, texCoords).a;
  vec3 fragAlbedo = texture(gAlbedoMetallic, texCoords).rgb;
  float fragMetallic = texture(gAlbedoMetallic, texCoords).a;
  vec3 fragEmission = texture(gEmission, texCoords).rgb;

  vec3 result;
  // Shade with normal lights.
  if (lightingModel == 0) {
    // Phong.
    result = qrk_shadeAllLightsBlinnPhongDeferred(
        fragAlbedo, /*specular=*/vec3(fragMetallic), ambient, shininess,
        fragPos_viewSpace, fragNormal_viewSpace);
  } else if (lightingModel == 1) {
    // GGX.
    result = qrk_shadeAllLightsCookTorranceGGXDeferred(
        fragAlbedo, ambient, fragRoughness, fragMetallic, fragPos_viewSpace,
        fragNormal_viewSpace);
  } else {
    // Invalid lighting model.
    result = vec3(1.0, 0.0, 0.0);
  }

  // Add emissions.
  result += qrk_shadeEmissionDeferred(fragEmission, fragPos_viewSpace,
                                      emissionAttenuation);

  result = qrk_toneMapReinhard(result);
  result = qrk_gammaCorrect(result);
  fragColor = vec4(result, 1.0);
}
