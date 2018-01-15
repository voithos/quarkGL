#version 330 core
#pragma qrk_include < post_processing.frag >
#pragma qrk_include < window.frag >
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

// TODO: Allow other post_processing methods from command line.
void main() {
  if (qrk_isWindowLeftHalf()) {
    if (qrk_isWindowTopHalf()) {
      fragColor = texture(screenTexture, texCoords);
    } else {
      fragColor = qrk_blurKernel(screenTexture, texCoords);
    }
  } else {
    if (qrk_isWindowTopHalf()) {
      fragColor = qrk_grayscale(texture(screenTexture, texCoords));
    } else {
      fragColor = qrk_edgeKernel(screenTexture, texCoords);
    }
  }
}
