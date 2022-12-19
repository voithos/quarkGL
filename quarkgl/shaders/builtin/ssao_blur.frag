#version 460 core
#pragma qrk_include < transforms.glsl>
#pragma qrk_include < window.frag>

in vec2 texCoords;

out float fragColor;

uniform sampler2D qrk_ssao;
uniform int qrk_ssaoNoiseTextureSideLength;

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(qrk_ssao, 0));

  float result = 0.0;
  // We compute a blur based on the original noise texture size used to generate
  // the SSAO.
  int sideOffset = qrk_ssaoNoiseTextureSideLength / 2;
  for (int x = -sideOffset; x < sideOffset; ++x) {
    for (int y = -sideOffset; y < sideOffset; ++y) {
      // Calculate the UV offset based on texel size.
      vec2 texOffset = vec2(float(x), float(y)) * texelSize;
      result += texture(qrk_ssao, texCoords + texOffset).r;
    }
  }
  // Normalize.
  result /= qrk_ssaoNoiseTextureSideLength * qrk_ssaoNoiseTextureSideLength;
  fragColor = result;
}
