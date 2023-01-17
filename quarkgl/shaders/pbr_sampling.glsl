#pragma once

#pragma qrk_include < constants.glsl>

/**
 * Samples the GGX distribution along a given normal direction based on a given
 * roughness. Xi should be a 2D random sampled number in the range [0..1].
 * Returns the sampled halfvector in the space of the given normal.
 */
vec3 qrk_importanceSampleGGX(vec3 normal, float roughness, vec2 Xi) {
  float a = roughness * roughness;

  // Our GGX is isotropic, so sample azimuth directly.
  float phi = 2.0 * PI * Xi.x;
  // (a^2 - 1) can be represented as (a-1)(a+1) which produces better floating
  // point precision.
  float cosTheta2 = (1.0 - Xi.y) / ((Xi.y * (a - 1)) * (a + 1) + 1.0);
  float cosTheta = sqrt(cosTheta2);
  float sinTheta = sqrt(1.0 - cosTheta2);

  // Convert from spherical to cartesian coordinates.
  vec3 halfVector = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

  // Convert from tangent space vector to the normal's space (usually, world
  // space). Handle the case of the normal being close to tangent "up".
  vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent = normalize(cross(up, normal));
  vec3 bitangent = cross(normal, tangent);

  mat3 TBN = mat3(tangent, bitangent, normal);
  return normalize(TBN * halfVector);
}