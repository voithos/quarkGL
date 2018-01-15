#ifndef QUARKGL_TRANSFORMS_GLSL_
#define QUARKGL_TRANSFORMS_GLSL_

/**
 * Calculates the normal of a triangle based on vertices.
 */
vec3 qrk_calculateNormal(vec3 x, vec3 y, vec3 z) {
  vec3 a = x - y;
  vec3 b = z - y;
  return normalize(cross(a, b));
}

#endif
