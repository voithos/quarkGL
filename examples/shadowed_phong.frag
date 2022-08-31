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

// TODO: Move into core lighting code.

/** Calculate a shadow bias based on the surface normal and light direction. */
float qrk_shadowBias(float minBias, float maxBias, vec3 normal, vec3 lightDir) {
  return max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
}

/**
 * Calculate whether the given fragment is in shadow.
 * Returns 1.0 if in shadow, 0.0 if not.
 */
float qrk_shadow(sampler2D shadowMap, vec4 fragPosLightSpace, float bias) {
  // Perform perspective divide.
  vec3 projectedPos = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // Shift to the range 0..1 so that we can compare with depth.
  projectedPos = projectedPos * 0.5 + 0.5;
  // Check for out-of-frustum.
  if (projectedPos.z > 1.0) {
    // Assume not in shadow.
    return 0.0;
  }
  float closestDepth = texture(shadowMap, projectedPos.xy).r;
  float currentDepth = projectedPos.z;
  // Check whether in shadow.
  return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main() {
  vec3 normal = normalize(fs_in.fragNormal);
  float shadowBias =
      qrk_shadowBias(0.001, 0.01, normal, qrk_directionalLights[0].direction);
  float shadow = qrk_shadow(shadowMap, fs_in.fragPosLightSpace, shadowBias);

  // Shade with normal lights.
  vec3 result = qrk_shadeAllLights(material, fs_in.fragPos, normal,
                                   fs_in.texCoords, shadow);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
