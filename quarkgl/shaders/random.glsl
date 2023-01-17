#pragma once

/** Common functions for [pseudo]random sequences. */

float qrk_VanDerCorputRadicalInverse(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10;  // / 0x100000000
}

/**
 * Returns the i'th number of the Hammersley sequence, out of N total numbers.
 * This represents a 2D sampling of a point in the range [0..1].
 */
vec2 qrk_hammersley(uint i, uint N) {
  return vec2(float(i) / float(N), qrk_VanDerCorputRadicalInverse(i));
}