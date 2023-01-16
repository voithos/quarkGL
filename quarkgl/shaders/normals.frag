#pragma once

/**
 * Samples a normal map and converts the texture colors [0..1] to a normalized
 * normal vector [-1..1], in tangent space.
 */
vec3 qrk_sampleNormalMap(sampler2D normalMap, vec2 texCoords) {
  return normalize(texture(normalMap, texCoords).xyz * 2.0 - 1.0);
}

/** Converts a normal to a color representation, with 100% opacity. */
vec4 qrk_normalColor(vec3 normal) { return vec4((normal + 1.0) / 2.0, 1.0); }