#version 460 core
#pragma qrk_include < gamma.frag>
#pragma qrk_include < tone_mapping.frag>

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D qrk_screenTexture;
uniform sampler2D qrk_bloom;
uniform bool bloom;
uniform float bloomMix;

uniform int toneMapping;
uniform bool gammaCorrect;
uniform float gamma;

void main() {
  vec3 color = texture(qrk_screenTexture, texCoords).rgb;
  if (bloom) {
    vec3 bloomColor = texture(qrk_bloom, texCoords).rgb;
    color = mix(color, bloomColor, bloomMix);
  }

  // Perform tone mapping.
  if (toneMapping == 1) {
    color = qrk_toneMapReinhard(color);
  } else if (toneMapping == 2) {
    color = qrk_toneMapReinhardLuminance(color);
  } else if (toneMapping == 3) {
    color = qrk_toneMapAcesApprox(color);
  } else if (toneMapping == 4) {
    color = qrk_toneMapAMD(color);
  } else {
    // No tone mapping.
  }

  // Perform gamma correction.
  if (gammaCorrect) {
    color = qrk_gammaCorrect(color, gamma);
  }

  fragColor = vec4(color, 1.0);
}
