#ifndef QUARKGL_TONE_MAPPING_FRAG_
#define QUARKGL_TONE_MAPPING_FRAG_

/**
 * Returns the luminance of a given color based on the sensitivity of the human
 * eye.
 */
float qrk_luminance(vec3 color) {
  return dot(color, vec3(0.2125, 0.7154, 0.0721));
}

/** Tone maps an HDR color using Reinhard tone mapping. */
vec3 qrk_toneMapReinhard(vec3 color) { return color / (color + 1.0f); }

/**
 * Tone maps an HDR color using Reinhard, but using a luminance-adjusted variant
 * for lower-band colors. Based on https://www.shadertoy.com/view/4dBcD1
 */
vec3 qrk_toneMapReinhardLuminance(vec3 color) {
  float luminance = qrk_luminance(color);
  vec3 mapped = qrk_toneMapReinhard(color);
  return mix(color / (1.0f + luminance), mapped, mapped);
}

/**
 * Simpler approximation to ACES, by Krzysztof Narkowicz.
 * https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
 */
vec3 qrk_toneMapAcesApprox(vec3 color) {
  color *= 0.6;
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0,
               1.0);
}

#endif
