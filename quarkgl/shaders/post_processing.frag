#ifndef QUARKGL_POST_PROCESSING_FRAG_
#define QUARKGL_POST_PROCESSING_FRAG_

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

/** Applies a blurring kernel. */
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

#endif
