#pragma once

/**
 * Calculates the normal of a triangle based on vertices.
 */
vec3 qrk_calculateNormal(vec3 x, vec3 y, vec3 z) {
  vec3 a = x - y;
  vec3 b = z - y;
  return normalize(cross(a, b));
}

/**
 * Calculates the TBN matrix based on a normal and tangent.
 * The normal and tangent should be normalized, but they do not need to be
 * orthonormal.
 */
mat3 qrk_calculateTBN(vec3 normal, vec3 tangent) {
  vec3 N = normal;
  vec3 T = tangent;
  // Make sure TBN is orthogonal.
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  return mat3(T, B, N);
}