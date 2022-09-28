#version 460 core
#define QRK_MAX_POINT_LIGHTS 32
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < lighting.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;
uniform sampler2D gEmission;

uniform float shininess;
uniform QrkAttenuation emissionAttenuation;

// World -> view space.
uniform mat4 view;

void main() {
  // Extract G-Buffer.
  vec3 fragPos_worldSpace = texture(gPosition, texCoords).rgb;
  vec3 fragNormal_worldSpace = texture(gNormal, texCoords).rgb;
  vec3 fragAlbedo = texture(gAlbedoSpecular, texCoords).rgb;
  vec3 fragSpecular = vec3(texture(gAlbedoSpecular, texCoords).a);
  vec3 fragEmission = texture(gEmission, texCoords).rgb;

  vec3 fragPos_viewSpace = vec3(view * vec4(fragPos_worldSpace, 1.0));
  vec3 fragNormal_viewSpace =
      vec3(transpose(inverse(view)) * vec4(fragNormal_worldSpace, 0.0));

  vec3 color = fragAlbedo * 0.05;  // Hard-coded ambient.
  // Shade with normal lights.
  color += qrk_shadeAllLightsDeferred(fragAlbedo, fragSpecular, shininess,
                                      fragPos_viewSpace, fragNormal_viewSpace);

  // Add emissions.
  color += qrk_shadeEmissionDeferred(fragEmission, fragPos_viewSpace,
                                     emissionAttenuation);

  color = qrk_toneMapAcesApprox(color);
  color = qrk_gammaCorrect(color);
  fragColor = vec4(color, 1.0);
}
