#pragma once

#ifndef QRK_KERNEL_GRANULARITY
#define QRK_KERNEL_GRANULARITY 1.0 / 300.0
#endif

vec2 qrk_kernelOffsets[9] = vec2[](
    vec2(-QRK_KERNEL_GRANULARITY, QRK_KERNEL_GRANULARITY),   // top-left
    vec2(0.0f, QRK_KERNEL_GRANULARITY),                      // top-center
    vec2(QRK_KERNEL_GRANULARITY, QRK_KERNEL_GRANULARITY),    // top-right
    vec2(-QRK_KERNEL_GRANULARITY, 0.0f),                     // center-left
    vec2(0.0f, 0.0f),                                        // center-center
    vec2(QRK_KERNEL_GRANULARITY, 0.0f),                      // center-right
    vec2(-QRK_KERNEL_GRANULARITY, -QRK_KERNEL_GRANULARITY),  // bottom-left
    vec2(0.0f, -QRK_KERNEL_GRANULARITY),                     // bottom-center
    vec2(QRK_KERNEL_GRANULARITY, -QRK_KERNEL_GRANULARITY)    // bottom-right
);

// Default gaussian weights, from center out.
float qrk_defaultGaussianWeights[5] =
    float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

/**
 * Applies a 3x3 kernel using a sampler2D and fragment coordinates.
 */
vec4 qrk_applyKernel(sampler2D screenTexture, vec2 texCoords, float kernel[9]) {
  vec3 samples[9];
  for (int i = 0; i < 9; i++) {
    samples[i] = vec3(texture(screenTexture, texCoords + qrk_kernelOffsets[i]));
  }

  vec3 color = vec3(0.0);
  for (int i = 0; i < 9; i++) {
    color += samples[i] * kernel[i];
  }
  return vec4(color, texture(screenTexture, texCoords).a);
}

/** Applies a sharpening kernel. */
vec4 qrk_sharpenKernel(sampler2D screenTexture, vec2 texCoords) {
  // clang-format off
  float kernel[9] = float[](
      -1, -1, -1,
      -1,  9, -1,
      -1, -1, -1
  );
  // clang-format on
  return qrk_applyKernel(screenTexture, texCoords, kernel);
}

/** Applies a simple blurring kernel. */
vec4 qrk_blurKernel(sampler2D screenTexture, vec2 texCoords) {
  // clang-format off
  float kernel[9] = float[](
      1.0 / 16, 2.0 / 16, 1.0 / 16,
      2.0 / 16, 4.0 / 16, 2.0 / 16,
      1.0 / 16, 2.0 / 16, 1.0 / 16
  );
  // clang-format on
  return qrk_applyKernel(screenTexture, texCoords, kernel);
}

/** Applies a edge detection kernel. */
vec4 qrk_edgeKernel(sampler2D screenTexture, vec2 texCoords) {
  // clang-format off
  float kernel[9] = float[](
      1,  1,  1,
      1, -8,  1,
      1,  1,  1
  );
  // clang-format on
  return qrk_applyKernel(screenTexture, texCoords, kernel);
}

/**
 * Inverts the given fragment color.
 */
vec4 qrk_invertColor(vec4 color) { return vec4(1.0 - color.rgb, color.a); }

/**
 * Grayscales the given fragment color, taking into account human eye
 * sensitivity.
 */
vec4 qrk_grayscale(vec4 color) {
  // Weighted average, based on the sensitivity of the human eye.
  float weightedAverage =
      0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  return vec4(weightedAverage, weightedAverage, weightedAverage, color.a);
}

/**
 * Grayscales the given fragment color. Does not take into account eye
 * sensitivity.
 */
vec4 qrk_grayscaleSimple(vec4 color) {
  float average = (color.r + color.g + color.b) / 3.0;
  return vec4(average, average, average, color.a);
}

/**
 * Performs a single pass of gaussian blur in either the x or y direction.
 */
vec4 qrk_gaussianBlurOnePass(sampler2D image, vec2 texCoords, bool horizontal) {
  vec2 texOffset = 1.0 / textureSize(image, /*mip=*/0);
  // Begin aggregating samples.
  // TODO: This doesn't handle alpha. Use premultiplied alpha?
  vec3 result = texture(image, texCoords).rgb * qrk_defaultGaussianWeights[0];
  if (horizontal) {
    for (int i = 1; i < 5; i++) {
      result += texture(image, texCoords + vec2(texOffset.x * i, 0.0)).rgb *
                qrk_defaultGaussianWeights[i];
      result += texture(image, texCoords - vec2(texOffset.x * i, 0.0)).rgb *
                qrk_defaultGaussianWeights[i];
    }
  } else {
    for (int i = 1; i < 5; i++) {
      result += texture(image, texCoords + vec2(0.0, texOffset.y * i)).rgb *
                qrk_defaultGaussianWeights[i];
      result += texture(image, texCoords - vec2(0.0, texOffset.y * i)).rgb *
                qrk_defaultGaussianWeights[i];
    }
  }
  return vec4(result, 1.0);
}