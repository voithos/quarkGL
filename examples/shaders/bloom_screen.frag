#version 460 core
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform bool useBloom;

void main() {
  // Additive blending.
  vec3 color = texture(screenTexture, texCoords).rgb;
  if (useBloom) {
    color += texture(bloomTexture, texCoords).rgb;
  }

  color = qrk_toneMapAcesApprox(color);
  color = qrk_gammaCorrect(color);
  fragColor = vec4(color, 1.0);
}
