#version 460 core
#pragma qrk_include < normals.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
// Which component of the G-Buffer to visualize.
uniform int gBufferVis;

void main() {
  vec4 color = texture(screenTexture, texCoords);

  if (gBufferVis == 1) {
    // Positions.
    fragColor = vec4(color.rgb, 1.0);
  } else if (gBufferVis == 2) {
    // AO.
    fragColor = vec4(color.a, color.a, color.a, 1.0);
  } else if (gBufferVis == 3) {
    // Normals.
    if (color.rgb == vec3(0.0)) {
      // Fragment has no normal info.
      fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
      fragColor = qrk_normalColor(color.rgb);
    }
  } else if (gBufferVis == 4) {
    // Roughness.
    fragColor = vec4(color.a, color.a, color.a, 1.0);
  } else if (gBufferVis == 5) {
    // Albedo.
    fragColor = vec4(color.rgb, 1.0);
  } else if (gBufferVis == 6) {
    // Metallic.
    fragColor = vec4(color.a, color.a, color.a, 1.0);
  } else if (gBufferVis == 7) {
    // Emission.
    fragColor = vec4(color.rgb, 1.0);
  } else {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
}
