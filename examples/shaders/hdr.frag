#version 460 core
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform bool useHdr;
uniform int toneMapTechnique;

void main() {
  vec3 color = texture(screenTexture, texCoords).rgb;

  if (useHdr) {
    if (toneMapTechnique == 0) {
      color = qrk_toneMapReinhard(color);
    } else if (toneMapTechnique == 1) {
      color = qrk_toneMapReinhardLuminance(color);
    } else if (toneMapTechnique == 2) {
      color = qrk_toneMapAcesApprox(color);
    }
  }

  color = qrk_gammaCorrect(color);
  fragColor = vec4(color, 1.0);
}
