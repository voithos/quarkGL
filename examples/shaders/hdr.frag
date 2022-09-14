#version 460 core
#pragma qrk_include < gamma.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform bool useHdr;

void main() {
  vec3 color = texture(screenTexture, texCoords).rgb;

  if (useHdr) {
    // Reinhard tone mapping.
    color = color / (color + vec3(1.0));
  }

  // Gamma correct.
  color = qrk_gammaCorrect(color);
  fragColor = vec4(color, 1.0);
}
