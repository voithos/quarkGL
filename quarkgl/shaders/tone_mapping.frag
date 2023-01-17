#pragma once

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

float qrk_toneMapAMDTermB(float hdrMax, float contrast, float shoulder,
                          float midIn, float midOut) {
  return (-pow(midIn, contrast) + pow(hdrMax, contrast) * midOut) /
         ((pow(pow(hdrMax, contrast), shoulder) -
           pow(pow(midIn, contrast), shoulder)) *
          midOut);
}

float qrk_toneMapAMDTermC(float hdrMax, float contrast, float shoulder,
                          float midIn, float midOut) {
  return (pow(pow(hdrMax, contrast), shoulder) * pow(midIn, contrast) -
          pow(hdrMax, contrast) * pow(pow(midIn, contrast), shoulder) *
              midOut) /
         ((pow(pow(hdrMax, contrast), shoulder) -
           pow(pow(midIn, contrast), shoulder)) *
          midOut);
}

// Build the following tonemap formula:
//   z = pow(x, contrast);
//   y = z / (pow(z, shoulder) * b + c);
//
// {contrast,shoulder} shapes the curve
// {b,c} anchors the curve
float qrk_toneMapAMDFormula(float x, vec4 p) {
  float z = pow(x, p.r);
  return z / (pow(z, p.g) * p.b + p.a);
}

/**
 * Tone mapper based on AMD's presentation.
 * Notably features channel crosstalk.
 * https://gpuopen.com/wp-content/uploads/2016/03/GdcVdrLottes.pdf
 */
vec3 qrk_toneMapAMD(vec3 color) {
  const float hdrMax = 16.0;   // How much HDR range before clipping. HDR modes
                               // likely need this pushed up to say 25.0.
  const float contrast = 2.0;  // Use as a baseline to tune the amount of
                               // contrast the tonemapper has.
  const float shoulder =
      1.0;  // Likely don't need to mess with this factor, unless matching
            // existing tonemapper is not working well...
  const float midIn = 0.18;   // Most games will have a {0.0 to 1.0} range for
                              // LDR so midIn should be 0.18.
  const float midOut = 0.18;  // Use for LDR. For HDR10 10:10:10:2 use maybe
                              // 0.18/25.0 to start. For scRGB, I forget what a
                              // good starting point is, need to re-calculate.

  float b = qrk_toneMapAMDTermB(hdrMax, contrast, shoulder, midIn, midOut);
  float c = qrk_toneMapAMDTermC(hdrMax, contrast, shoulder, midIn, midOut);

  const float EPS = 1e-6f;
  float peak = max(color.r, max(color.g, color.b));
  peak = max(EPS, peak);

  vec3 ratio = color / peak;
  peak = qrk_toneMapAMDFormula(peak, vec4(contrast, shoulder, b, c));

  float crosstalk = 4.0;        // controls amount of channel crosstalk
  float saturation = contrast;  // full tonal range saturation control
  float crossSaturation = contrast * 16.0;  // crosstalk saturation

  float white = 1.0;

  // Wrap crosstalk in transform.
  ratio = pow(abs(ratio), vec3(saturation / crossSaturation));
  ratio = mix(ratio, vec3(white), vec3(pow(peak, crosstalk)));
  ratio = pow(abs(ratio), vec3(crossSaturation));

  // Then apply ratio to peak.
  color = peak * ratio;
  return color;
}