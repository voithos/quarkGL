#pragma once

#ifndef QUARKGL_GAMMA
#define QUARKGL_GAMMA 2.2
#endif

/**
 * Apply gamma correction to a given color. Can be used after fragment shader
 * lighting to perform gamma correction:
 *
 *   fragColor.rgb = qrk_gammaCorrect(fragColor.rgb, 2.2);
 */
vec3 qrk_gammaCorrect(vec3 color, float gamma) {
  return pow(color, vec3(1.0 / gamma));
}

/**
 * Apply gamma correction to a given color. Can be used after fragment shader
 * lighting to perform gamma correction:
 *
 *   fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
 */
vec3 qrk_gammaCorrect(vec3 color) {
  return qrk_gammaCorrect(color, QUARKGL_GAMMA);
}